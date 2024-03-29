#include "clang/Driver/Options.h"
#include "clang/AST/AST.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/ASTConsumers.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/AST/Stmt.h"
#include "clang/AST/Expr.h"
#include "OMPAdvisor.h"

using namespace clang;
using namespace std;

/***************************** OMPAdvisorVisitor *****************************/

OMPAdvisorVisitor::OMPAdvisorVisitor(CompilerInstance *CI)
  : astContext(&(CI->getASTContext())),
    SM(&(CI->getASTContext().getSourceManager())),
    isBefore(*SM) {
  insideLoop = false;                                                       
  insideKernel = false;                                                     
  lastKernel = NULL;
  num_collapse = 0;                                                         
}

bool OMPAdvisorVisitor::isWithin(SourceLocation src) {
    if(!lastKernel) return false;

    OMPExecutableDirective *Dir = dyn_cast<OMPExecutableDirective> (lastKernel->getStmt());
    CapturedStmt *cs;
    for(auto c : Dir->children()) {
        cs = dyn_cast<CapturedStmt>(c);
//        if(DEBUG) cs->dump();
        break;
    }
    if(!cs) return false;
    if(isBefore(lastKernel->getStmt()->getBeginLoc(), src) &&
            isBefore(src, cs->getEndLoc()))
        return true;
    return false;
}

bool OMPAdvisorVisitor::isNotPrivate(ValueDecl* v) {
    for(VarDecl* var: lastKernel->getPrivate()) {
        if(v == var) {
            if(DEBUG) llvm::errs() << v->getNameAsString() << " is private\n";
            return false;
        }
    }
    return true;
}

map<int, vector<Kernel*>> OMPAdvisorVisitor::getKernelMap() {
  return kernel_map;
}

map<int, vector<Loop*>> OMPAdvisorVisitor::getLoopMap() {
  return loop_map;
}

bool OMPAdvisorVisitor::VisitFunctionDecl(FunctionDecl *FD) {
  currentFunction = FD;
//  llvm::outs() << "Function - " << FD->getNameInfo().getAsString() << "\n";
  return true;
}

ValueDecl *OMPAdvisorVisitor::getLeftmostNode(Stmt *st) {
    Stmt* q = st;
    while(q != NULL) {
        Stmt* b = NULL;
        for(Stmt *a: q->children()) {
            if(DeclRefExpr *d = dyn_cast<DeclRefExpr>(a)) {
                if(DEBUG) {
                    llvm::errs() << "  |- Leftmost node = ";
                    llvm::errs() << d->getDecl()->getNameAsString();
                    llvm::errs() << "\n";
                }
                return d->getDecl();
            }
            b = a;
            break;
        }
        q = b;
    }
    return NULL;
}

bool OMPAdvisorVisitor::VisitStmt(Stmt *st) {
  // Ignore if the statement is in System Header files                        
  if(!st->getBeginLoc().isValid() || SM->isInSystemHeader(st->getBeginLoc()))                            
    return true;                                                            
                                                                                
  if(insideLoop && isBefore(loopEnd, st->getBeginLoc())) {                    
    insideLoop = false;                                                     
  }                    

  //st->dumpColor();
  bool found = kernel_found<OMPTargetDirective>(st);
  if(found) {
//    st->dumpColor();
    int id = lastKernel ? lastKernel->getID() + 1 : 1;
    lastKernel = new Kernel(id, st, currentFunction); 
    llvm::errs().changeColor(raw_ostream::GREEN);
    llvm::errs() << "INFO: Kernel found in '" << currentFunction->getNameInfo().getAsString() << "' at ";
    st->getBeginLoc().print(llvm::errs(), *SM);
    llvm::errs() << "\n";
    llvm::errs().resetColor();
    lastKernel->setCodeLocation(0, st->getBeginLoc());
    lastKernel->setCodeLocation(1, st->getBeginLoc());

    lastKernel->setEndLoc(dyn_cast<OMPTargetDirective>(st)->getInnermostCapturedStmt()->getEndLoc());

    if(insideLoop) {
        lastKernel->setInLoop(true);
        lastLoop->addKernel(lastKernel->getID());
        lastKernel->setLoop(lastLoop);
    }
    vector<Kernel*> vec;
    vec.push_back(lastKernel);
    kernel_map[id] = vec;

    return true;
  }

  if(dyn_cast<ForStmt>(st)) {
    if(isWithin(st->getBeginLoc())) {
      int x = SM->getSpellingColumnNumber(st->getBeginLoc());
      if(lastKernel->getCodeLocation(2).isInvalid()) {
        lastKernel->setCodeLocation(2, st->getBeginLoc().getLocWithOffset(1-x));          // For Loop 1
        lastKernel->setCodeLocation(6, st->getEndLoc());
        ForStmt *forStmt = dyn_cast<ForStmt>(st);
        if(dyn_cast<ForStmt>(dyn_cast<CompoundStmt>(forStmt->getBody())->body_front())) {
          ForStmt *forStmt2 = dyn_cast<ForStmt>(dyn_cast<CompoundStmt>(forStmt->getBody())->body_front());
          lastKernel->setCodeLocation(3, forStmt2->getBeginLoc().getLocWithOffset(1-x));   // For Loop 2
          if(dyn_cast<ForStmt>(dyn_cast<CompoundStmt>(forStmt2->getBody())->body_front())) {
            ForStmt *forStmt3 = dyn_cast<ForStmt>(dyn_cast<CompoundStmt>(forStmt2->getBody())->body_front());
            lastKernel->setCodeLocation(4, forStmt3->getBeginLoc().getLocWithOffset(1-x));   // For Loop 3
            if(dyn_cast<ForStmt>(dyn_cast<CompoundStmt>(forStmt3->getBody())->body_front())) {
              ForStmt *forStmt4 = dyn_cast<ForStmt>(dyn_cast<CompoundStmt>(forStmt3->getBody())->body_front());
              lastKernel->setCodeLocation(5, forStmt4->getBeginLoc().getLocWithOffset(1-x));   // For Loop 4
            }
          }
        }
      }
    }
    int loopID = lastLoop ? lastLoop->getID() + 1 : 1;
    Loop *l = new Loop(loopID, st);
    if(!insideLoop) {
      loopEnd = st->getEndLoc();
      l->setStartLoc(st->getBeginLoc());
      l->setEndLoc(loopEnd);
      lastLoop = l;
    } else {
      l->setStartLoc(lastLoop->getStartLoc());
      l->setEndLoc(lastLoop->getEndLoc());
    }
    insideLoop = true;
    vector<Loop*> vec;
    vec.push_back(l);
    loop_map[loopID] = vec;
  } else if(DeclRefExpr *d = dyn_cast<DeclRefExpr>(st)) {
    if(!dyn_cast<clang::FunctionDecl>(d->getDecl())) {
      if(ValueDecl *v = d->getDecl()) {
        if(DEBUG) {
          llvm::errs() << "variable - ";
          llvm::errs() << v->getNameAsString();
          llvm::errs() << "\n"; // DEBUG
        }
        if(isWithin(d->getBeginLoc())) {
          if(isNotPrivate(v)) {
            if(lastKernel->isInLoop())
              lastKernel->getLoop()->addValueIn(v);
            else
              lastKernel->addValueIn(v);
          }
        }
      }
    }
  } else if(BinaryOperator *b = dyn_cast<BinaryOperator>(st)) {
    if(b->isAssignmentOp()) {
      if(DEBUG) {
        llvm::errs() << b->getOpcodeStr() << " ";
        //b->getOperatorLoc().dump(*SM);
      }
      if(isWithin(b->getBeginLoc())) {
        ValueDecl *v = getLeftmostNode(st);
        if(isNotPrivate(v)) {
          if(lastKernel->isInLoop())
            lastKernel->getLoop()->addValueIn(v);
          else
            lastKernel->addValueOut(v);
        }
      }
    } else {
      if(DEBUG) {
        llvm::errs() << b->getOpcodeStr();
        llvm::errs() << "\n";
      }
    }
  } else if(UnaryOperator *u = dyn_cast<UnaryOperator>(st)) {
    if(u->isPostfix() || u->isPrefix()) {
      if(DEBUG) {
        llvm::errs() << UnaryOperator::getOpcodeStr(u->getOpcode());
        llvm::errs() <<" u ";
        //u->getOperatorLoc().dump(*SM);
      }
      if(isWithin(u->getBeginLoc())) {
        ValueDecl *v = getLeftmostNode(st);
        if(isNotPrivate(v)) {
          if(lastKernel->isInLoop())
            lastKernel->getLoop()->addValueIn(v);
          else
            lastKernel->addValueOut(v);
        }
      }
    } else {
      if(DEBUG) {
        llvm::errs() << UnaryOperator::getOpcodeStr(u->getOpcode());
        llvm::errs() <<" u\n";
      }
    }
  }

  return true;
}
