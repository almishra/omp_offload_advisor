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
#include "clang/Format/Format.h"
#include <sstream>
#include "OMPAdvisor.h"

using namespace clang;
using namespace std;

std::string CODE_DEC_TIMEVAL = "struct timeval tv1, tv2;\n";
std::string CODE_GETTIME_1 = "gettimeofday(&tv1, NULL);\n";
std::string CODE_START = "long start = (tv1.tv_sec*1000000 + tv1.tv_usec);\n";
std::string CODE_GETTIME_2 = "gettimeofday(&tv2, NULL);\n";
std::string CODE_END = "long end = (tv2.tv_sec*1000000 + tv2.tv_usec);\n";
std::string CODE_DIST = "#pragma omp target teams distribute";
std::string CODE_ENTER_DATA = "#pragma omp target enter data";
std::string CODE_EXIT_DATA = "\n#pragma omp target exit data";
std::string CODE_PARALLEL_FOR = "#pragma omp parallel for";
const char* schedule[3] = {"static", "dynamic", "guided"};

/***************************** OMPAdvisorASTConsumer *****************************/

OMPAdvisorASTConsumer::OMPAdvisorASTConsumer(CompilerInstance *CI) {
  visitor = new OMPAdvisorVisitor(CI);
  ci = CI;
  SM = &(CI->getASTContext().getSourceManager());
  for(int i=0; i<84; i++) {
    rewriter[i].setSourceMgr(*SM, CI->getASTContext().getLangOpts());
  }
  llvm::outs() << "Original File:\n";
  llvm::outs() << "******************************\n";
  rewriter[0].getEditBuffer(SM->getMainFileID()).write(llvm::outs());
  llvm::outs() << "******************************\n";
}

string OMPAdvisorASTConsumer::basename(string path) {                    
  return string(find_if(path.rbegin(), path.rend(),
                MatchPathSeparator()).base(), path.end());                      
} 

void OMPAdvisorASTConsumer::HandleTranslationUnit(ASTContext &Context) {
  llvm::outs() << "******************************\n";
  llvm::outs() << "OMPAdvisor Plugin\n";
  llvm::outs() << "******************************\n";
  visitor->TraverseDecl(Context.getTranslationUnitDecl());
  kernel_map = visitor->getKernelMap();
  loop_map  = visitor->getLoopMap();
  getKernelInfo();
  getLoopInfo();

  string filename = basename(SM->getFilename(SM->getLocForStartOfFile(SM->getMainFileID())).str());
  llvm::outs() << "Filename = " << filename << "\n";
  size_t lastindex = filename.find_last_of(".");
  string directory_name = filename.substr(0, lastindex) + "_variants";
  if(!llvm::sys::fs::exists(directory_name)) {
    llvm::sys::fs::create_directories(directory_name);
  } else {
    llvm::errs() << "Directory " << directory_name << " already exists\n";
  }

  for(auto m = kernel_map.begin(); m != kernel_map.end(); m++) {
    Kernel *k = m->second.at(0);

    int num_par = 0;
    if(k->getCodeLocation(2).isValid()) num_par++;
    if(k->getCodeLocation(3).isValid()) num_par++;
    if(k->getCodeLocation(4).isValid()) num_par++;
    if(k->getCodeLocation(5).isValid()) num_par++;
    int rID = 0;
    string newFile = directory_name + "/" + filename.substr(0, lastindex);
  //  for(int num_par=1; num_par<=4; num_par++) {
  //  for(int num_par=1; num_par<=1; num_par++) {
    for(int parallel=2; parallel<=num_par+1; parallel++) {
      int max = (parallel==2) ? 1 : (parallel-2);
      for(int dist_col=1; dist_col<=max; dist_col++) {
        for(int col=1; col<=num_par+2-parallel; col++) {
          for(int s=0; s<3; s++) {
            for(int d=0; d<2; d++) {
              codeGen(newFile, k, rID++, num_par, parallel, dist_col, col, s, d);
            }
          }
        }
      }
    }
   // }                                             
  }

  llvm::outs() << "\n\nModified File:\n";
  llvm::outs() << "******************************\n";
  rewriter[0].getEditBuffer(SM->getMainFileID()).write(llvm::outs());
  llvm::outs() << "******************************\n";
  rewriter[1].getEditBuffer(SM->getMainFileID()).write(llvm::outs());
  llvm::outs() << "******************************\n";
}

void OMPAdvisorASTConsumer::codeGen(string fileName, Kernel *k, int rID, int num_par, int par_pos, int dist_col, int par_col, int sched, int data) {
  std::ostringstream sstream;
  sstream << fileName << "_" << k->getFunction()->getNameInfo().getAsString() << "_" << k->getID() << "_" << par_pos << "_" << dist_col << "_" << par_col << "_" << sched << "_" << data <<".cpp";
  fileName = sstream.str();

  string code0;
  string code1;
  if(data == 0) {
    code0 = CODE_DEC_TIMEVAL + CODE_GETTIME_1 + CODE_START;
    code1 = CODE_ENTER_DATA + " map(alloc: A[0:N][0:N])\n";
  } else {
    code0 = CODE_ENTER_DATA + " map(alloc: A[0:N][0:N])\n";
    code1 = CODE_DEC_TIMEVAL + CODE_GETTIME_1 + CODE_START;
  }
  string code2 = CODE_DIST;
  string code3 = "";
  string code4 = "";
  string code5 = "";
  if(dist_col > 1) code2 += " collapse(" + std::to_string(dist_col) + ")";
  if(par_pos == 2) {
    code2 += " parallel for";
    if(par_col > 1) code2 += " collapse(" + std::to_string(par_col) + ")";
    if(sched > 0) code2 += " schedule(" + string(schedule[sched]) + ")";
  }
  code2 += "\n";
  if(par_pos == 3) {
    code3 += CODE_PARALLEL_FOR;
    if(par_col > 1) code3 += " collapse(" + std::to_string(par_col) + ")";
    if(sched > 0) code3 += " schedule(" + string(schedule[sched]) + ")";
    code3 += "\n";
  }
  if(par_pos == 4) {
    code4 += CODE_PARALLEL_FOR;
    if(par_col > 1) code4 += " collapse(" + std::to_string(par_col) + ")";
    if(sched > 0) code4 += " schedule(" + string(schedule[sched]) + ")";
    code4 += "\n";
  }
  if(par_pos == 5) {
    code5 += CODE_PARALLEL_FOR;
    if(par_col > 1) code5 += " collapse(" + std::to_string(par_col) + ")";
    if(sched > 0) code5 += " schedule(" + string(schedule[sched]) + ")";
    code5 += "\n";
  }

  string code6 = CODE_EXIT_DATA + " map(from: A[0:N][0:N])\n";
  string code7 = CODE_GETTIME_2;
  code7 += CODE_END;
  code7 += "printf(\"";
  code7 += k->getFunction()->getNameInfo().getAsString();
  code7 += "_parPos" + std::to_string(par_pos);  
  code7 += "_distCol" + std::to_string(dist_col);  
  code7 += "_parCol" + std::to_string(par_col);  
  code7 += "_" + string(schedule[sched]);
  code7 += ",%ld\\n\",  (end - start));\n";
  if(data == 1) {
    string temp = code6;
    code6 = code7;
    code7 = temp;
  }

  rewriter[rID].RemoveText(k->getStmt()->getSourceRange());
  rewriter[rID].InsertTextAfterToken(k->getCodeLocation(6), code6);
  rewriter[rID].InsertTextAfterToken(k->getCodeLocation(6), code7);
  if(k->getCodeLocation(5).isValid())
    rewriter[rID].InsertTextBefore(k->getCodeLocation(5), code5);
  if(k->getCodeLocation(4).isValid())
    rewriter[rID].InsertTextBefore(k->getCodeLocation(4), code4);
  if(k->getCodeLocation(3).isValid())
    rewriter[rID].InsertTextBefore(k->getCodeLocation(3), code3);
  if(k->getCodeLocation(2).isValid())
    rewriter[rID].InsertTextBefore(k->getCodeLocation(2), code2);
  if(k->getCodeLocation(1).isValid())
    rewriter[rID].InsertTextBefore(k->getCodeLocation(1), code1);
  if(k->getCodeLocation(0).isValid())
    rewriter[rID].InsertTextBefore(k->getCodeLocation(0), code0);

  llvm::errs() << "Modified File at " << fileName << "\n";
  std::error_code OutErrorInfo;
  std::error_code ok;
  llvm::raw_fd_ostream outFile(llvm::StringRef(fileName),
      OutErrorInfo, llvm::sys::fs::CD_CreateAlways);
  if (OutErrorInfo == ok) {
    const RewriteBuffer *RewriteBuf = rewriter[rID].getRewriteBufferFor(SM->getMainFileID());
    outFile << std::string(RewriteBuf->begin(), RewriteBuf->end());
  } else {
    llvm::errs() << "Could not create file\n";
  }
  outFile.close();
  system(("clang-format -i " + fileName).c_str());
}

void OMPAdvisorASTConsumer::getKernelInfo() {
  llvm::errs() << "Kernel Data Information\n";
  llvm::errs() << "Total " << kernel_map.size() << " kernels:\n";
  for(auto m = kernel_map.begin(); m != kernel_map.end(); m++) {
//    int id = m->first;
    Kernel *k = m->second.at(0);
    llvm::errs() << "Kernel #" << k->getID() << "\n";
    llvm::errs() << " Function: " << k->getFunction()->getNameAsString();
    llvm::errs() << "\n Location: ";
    k->getStmt()->getBeginLoc().print(llvm::errs(), *SM); 
    llvm::errs() << "\n In Loop: ";
    if(k->isInLoop()) {
      llvm::errs() << "Yes ";
      k->getLoop()->getStartLoc().print(llvm::errs(), *SM);
    } else {
      llvm::errs() << "No";
    }
    llvm::errs() << "\n";
    llvm::errs() << " Data used\n";
    if(k->getPrivate().size()) llvm::errs() << "  private:\n";
    for(VarDecl *v : k->getPrivate()) {
      llvm::errs() << "   |-  " << v->getNameAsString() << "\n";
    }
    if(k->getValIn().size()) llvm::errs() << "  to:\n";
    for(ValueDecl *v : k->getValIn()) {
      llvm::errs() << "   |-  " << v->getNameAsString() << "\n";
    }
    if(k->getValOut().size()) llvm::errs() << "  from:\n";
    for(ValueDecl *v : k->getValOut()) {
        llvm::errs() << "   |-  " << v->getNameAsString() << "\n";
    }
    if(k->getValInOut().size()) llvm::errs() << "  tofrom:\n";
    for(ValueDecl *v : k->getValInOut()) {
        llvm::errs() << "   |-  " << v->getNameAsString() << "\n";
    }
    if(k->getSharedValIn().size()) llvm::errs() << "  shared to:\n";
    for(ValueDecl *v : k->getSharedValIn()) {
        llvm::errs() << "   |-  " << v->getNameAsString() << "\n";
    }
    if(k->getSharedValOut().size()) llvm::errs() << "  shared from:\n";
    for(ValueDecl *v : k->getSharedValOut()) {
        llvm::errs() << "   |-  " << v->getNameAsString() << "\n";
    }
    if(k->getSharedValInOut().size()) llvm::errs() << "  shared tofrom:\n";
    for(ValueDecl *v : k->getSharedValInOut()) {
        llvm::errs() << "   |-  " << v->getNameAsString() << "\n";
    }
    llvm::errs() << "\n";

/*    rewriter[0].RemoveText(k->getStmt()->getSourceRange());
    rewriter[0].InsertTextAfterToken(k->getCodeLocation(6), "\n// Position 6 for target exit data map\n");
    if(k->getCodeLocation(5).isValid())
      rewriter[0].InsertTextBefore(k->getCodeLocation(5), "// Position 5 for parallel for\n");
    if(k->getCodeLocation(4).isValid())
      rewriter[0].InsertTextBefore(k->getCodeLocation(4), "// Position 4 for parallel for\n");
    if(k->getCodeLocation(3).isValid())
      rewriter[0].InsertTextBefore(k->getCodeLocation(3), "// Position 3 for parallel for\n");
    if(k->getCodeLocation(2).isValid())
      rewriter[0].InsertTextBefore(k->getCodeLocation(2), "// Position 2 for target teams distribute\n");
    rewriter[0].InsertTextBefore(k->getCodeLocation(1), "// Position 1 for target enter data map\n");
    rewriter[0].InsertTextBefore(k->getCodeLocation(0), "// Position 0 for comment\n");
*/
  }
}

void OMPAdvisorASTConsumer::getLoopInfo() {
  llvm::errs() << "Loop Information\n";
  for(auto m : kernel_map) {
    Kernel *k = m.second.at(0);
    if(k->isInLoop()) {
      m.second.push_back(k);                                              
      k->setLink(k->getID());                                             
      k->setStartLoc(k->getLoop()->getStartLoc());                        
      k->setEndLoc(k->getLoop()->getEndLoc());
    } else {
      k->setStartLoc(k->getStmt()->getBeginLoc());
      OMPExecutableDirective *Dir =
                      dyn_cast<OMPExecutableDirective>(k->getStmt());
      CapturedStmt *cs;
      for(auto c : Dir->children()) {
        cs = dyn_cast<CapturedStmt>(c);
        break;
      }
      k->setEndLoc(cs->getEndLoc());
    }

    llvm::errs() << "Kernel #" << k->getID();                               
    if(k->isInLoop()) llvm::errs() << "  -- In Loop";
    llvm::errs() << "\nStart: ";
    k->getStartLoc().print(llvm::errs(), *SM);
    llvm::errs() << "\nEnd  : ";
    k->getEndLoc().print(llvm::errs(), *SM);
    llvm::errs() << "\n";
    llvm::errs() << "\n";
  }
  llvm::errs() << "********************************\n";
  for(auto m : loop_map) {
    Loop *l = m.second.at(0);
    llvm::errs() << "Loop ID: " << l->getID() << " - ";
    for(int id : l->getKernels()) {
      llvm::errs() << id << ",";
    }
    llvm::errs() << "\nStart Loc: ";
    l->getStartLoc().print(llvm::errs(), *SM);
    llvm::errs() << "\nEnd Loc: ";
    l->getEndLoc().print(llvm::errs(), *SM);
    llvm::errs() << "\n";
    if(l->getValIn().size()) llvm::errs() << "  to:\n";
    for(ValueDecl *v : l->getValIn()) {
      llvm::errs() << "   |-  " << v->getNameAsString() << "\n";
    }
    if(l->getValOut().size()) llvm::errs() << "  from:\n";
    for(ValueDecl *v : l->getValOut()) {
      llvm::errs() << "   |-  " << v->getNameAsString() << "\n";
    }
    if(l->getValInOut().size()) llvm::errs() << "  tofrom:\n";
    for(ValueDecl *v : l->getValInOut()) {
      llvm::errs() << "   |-  " << v->getNameAsString() << "\n";
    }
  }
}

