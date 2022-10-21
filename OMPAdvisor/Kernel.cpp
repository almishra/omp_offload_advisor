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

Kernel::Kernel(int ID, Stmt *stmt, FunctionDecl *F) : id(ID) {
  st = stmt;
  FD = F;
  inLoop = false;
  loop = NULL;
  link = 0;
//  llvm::errs() << "Code Source Locations\n";
//  for(int i=0; i<8; i++) {
//    llvm::errs() << i << " - " << (codeLoc[i].isValid() ? "True\n" : "False\n");
//  }
}

int Kernel::getID() const {
  return id;
}

SourceLocation Kernel::getCodeLocation(int N) {
  if(N<0 || N>6) {
    llvm::errs() << "Wrong code location\n";
    llvm::errs() << "Code index should be 0 <= N <= 6\n";
    return codeLoc[7];
  }
  return codeLoc[N];
}

void Kernel::setCodeLocation(int N, SourceLocation Loc) {
  if(N<0 || N>6) {
    llvm::errs() << "Wrong code index " << N << "\n";
    llvm::errs() << "Code index should be 0 <= N <= 6\n";
  } else {
    codeLoc[N] = Loc;
//    llvm::errs() << "Code Source Locations\n";
//    for(int i=0; i<8; i++) {
//      llvm::errs() << i << " - " << (codeLoc[i].isValid() ? "True\n" : "False\n");
//    }
  }
}

int Kernel::getNumNestedLoop() {
  return numNestedLoop;
}

void Kernel::findNestedLoops() {

}

void Kernel::setInLoop(bool in) {
  inLoop = in;
}

bool Kernel::isInLoop() {
  return inLoop;
}

Stmt* Kernel::getStmt() {
  return st;
}

Loop* Kernel::getLoop() {
  return loop;
}

void Kernel::setLoop(Loop *l) {
  loop = l;
  loop->addKernel(id);
}

FunctionDecl* Kernel::getFunction() {
  return FD;
}

void Kernel::setFuction(FunctionDecl *F) {
  FD = F;
}

void Kernel::addPrivate(VarDecl *d) {
  privList.insert(d);
}

set<VarDecl*> Kernel::getPrivate() {
  return privList;
}

void Kernel::removePrivate(VarDecl *d) {
  privList.erase(d);
}

void Kernel::addValueIn(ValueDecl *d) {
  valIn.insert(d);
}

void Kernel::removeValueIn(ValueDecl *d) {
  valIn.erase(d);
}

set<ValueDecl*> Kernel::getValIn() {
  return valIn;
}

void Kernel::addValueOut(ValueDecl *d) {
  valOut.insert(d);
}

void Kernel::removeValueOut(ValueDecl *d) {
  valOut.erase(d);
}

set<ValueDecl*> Kernel::getValOut() {
  return valOut;
}

void Kernel::addValueInOut(ValueDecl *d) {
  valInOut.insert(d);
}

void Kernel::removeValueInOut(ValueDecl *d) {
  valInOut.erase(d);
}

set<ValueDecl*> Kernel::getValInOut() {
  return valInOut;
}

void Kernel::addSharedValueIn(ValueDecl *d) {
  sharedValIn.insert(d);
}

void Kernel::removeSharedValueIn(ValueDecl *d) {
  sharedValIn.erase(d);
}

set<ValueDecl*> Kernel::getSharedValIn() {
  return sharedValIn;
}

void Kernel::addSharedValueOut(ValueDecl *d) {
  sharedValOut.insert(d);
}

void Kernel::removeSharedValueOut(ValueDecl *d) {
  sharedValOut.erase(d);
}

set<ValueDecl*> Kernel::getSharedValOut() {
  return sharedValOut;
}

void Kernel::addSharedValueInOut(ValueDecl *d) {
  sharedValInOut.insert(d);
}

void Kernel::removeSharedValueInOut(ValueDecl *d) {
  sharedValInOut.erase(d);
}

set<ValueDecl*> Kernel::getSharedValInOut() {
  return sharedValInOut;
}

void Kernel::setStartLoc(SourceLocation start) {
  startLoc = start;
}

SourceLocation Kernel::getStartLoc() {
  return startLoc;
}

void Kernel::setEndLoc(SourceLocation end) {
  endLoc = end;
}

SourceLocation Kernel::getEndLoc() {
  return endLoc;
}

void Kernel::setLink(int id) {
  link = id;
}

int Kernel::isLinkedTo() {
  return link;
}

bool Kernel::operator< (const Kernel& k) const {
  return this->id < k.id;
}

bool Kernel::operator> (const Kernel& k) const {
  return this->id > k.id;
}
