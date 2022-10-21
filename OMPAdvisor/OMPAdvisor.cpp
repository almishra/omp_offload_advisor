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

unique_ptr<ASTConsumer> PluginOMPAdvisor::CreateASTConsumer(CompilerInstance &CI, llvm::StringRef file) {
  return make_unique<OMPAdvisorASTConsumer>(&CI);
}

bool PluginOMPAdvisor::ParseArgs(const CompilerInstance &CI, const vector<string> &args) {
  if (args.size() && args[0] == "help") {
    PrintHelp(llvm::errs());
    return false;
  }

  return true;
}

void PluginOMPAdvisor::PrintHelp(llvm::raw_ostream& os) {
  os << "Help for OMP Static Advisor Plugin\n";
}

static FrontendPluginRegistry::Add<PluginOMPAdvisor>
X("omp-advisor", "OMP Static Advisor Plugin");
