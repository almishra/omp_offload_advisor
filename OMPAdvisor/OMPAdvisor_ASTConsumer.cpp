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
std::string CODE_START_TIME = "long start = (tv1.tv_sec*1000000 + tv1.tv_usec);\n";
std::string CODE_GETTIME_2 = "gettimeofday(&tv2, NULL);\n";
std::string CODE_END_TIME = "long end = (tv2.tv_sec*1000000 + tv2.tv_usec);\n";
std::string CODE_DIST = "#pragma omp target teams distribute";
std::string CODE_ENTER_DATA = "#pragma omp target enter data";
std::string CODE_EXIT_DATA = "\n#pragma omp target exit data";
std::string CODE_PARALLEL_FOR = "#pragma omp parallel for";
const char* schedule[3] = {"static", "dynamic", "guided"};

/***************************** OMPAdvisorASTConsumer *****************************/

OMPAdvisorASTConsumer::OMPAdvisorASTConsumer(CompilerInstance *CI) {
  visitor = new OMPAdvisorVisitor(CI);
  astContext = &(CI->getASTContext());
  ci = CI;
  SM = &(astContext->getSourceManager());
  for(int i=0; i<84; i++) {
    rewriter[i].setSourceMgr(*SM, CI->getASTContext().getLangOpts());
  }
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
//  loop_map  = visitor->getLoopMap();
//  getKernelInfo();
//  getLoopInfo();

  string filename = basename(SM->getFilename(SM->getLocForStartOfFile(SM->getMainFileID())).str());
  llvm::outs() << "Filename = " << filename << "\n";
  string directory = SM->getFileEntryForID(SM->getMainFileID())->tryGetRealPathName().str();
  size_t lastindex = directory.find_last_of(".");
  string directory_name = directory.substr(0, lastindex) + "_variants";
  llvm::outs() << "Directory= " << directory << "\n";
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
    lastindex = filename.find_last_of(".");
    string newFile = directory_name + "/" + filename.substr(0, lastindex);
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
  }
}

string OMPAdvisorASTConsumer::getArraySize(OMPArraySectionExpr *array, string &size) {
  Expr::EvalResult result;
  int lower = 0, length = 0, stride = 0;
  Expr *temp;
  temp = array->getLowerBound();
  if(temp) {
    temp->EvaluateAsInt(result, *astContext);
    if(result.Val.isInt()) {
      lower = result.Val.getInt().getLimitedValue();
    }
  }
  temp = array->getLength();
  if(temp) {
    temp->EvaluateAsInt(result, *astContext);
    if(result.Val.isInt()) {
      length = result.Val.getInt().getLimitedValue();
    }
  }
  string curSize = "[" + std::to_string(lower) + ":" + std::to_string(length);
  temp = array->getStride();
  if(temp) {
    temp->EvaluateAsInt(result, *astContext);
    if(result.Val.isInt()) {
      stride = result.Val.getInt().getLimitedValue();
      curSize += ":" + std::to_string(stride);
    }
  }
  curSize += "]";
  size = curSize + size;

  Expr *exp = array->getBase();
  string varName = "";
  if(auto array1 = dyn_cast<OMPArraySectionExpr>(exp)) {
    varName = getArraySize(array1, size);
  } else if(auto impl = dyn_cast<ImplicitCastExpr>(exp)) {
    DeclRefExpr *ref = dyn_cast<DeclRefExpr>(impl->getSubExpr());
    varName = ref->getDecl()->getNameAsString();
  }

  return varName;
}

string OMPAdvisorASTConsumer::getEnterDataString(Kernel *k) {
  string enterData = "";
  OMPTargetDirective *omp = dyn_cast<OMPTargetDirective>(k->getStmt());
  if(omp->hasClausesOfKind<OMPMapClause>()) {
    enterData = "map(to: ";
    int numClauses = omp->getNumClauses();
    for(int i=0; i<numClauses; i++) {
      OMPClause *c = omp->getClause(i);
      if(dyn_cast<OMPMapClause>(c)) {
        OMPMapClause *map = dyn_cast<OMPMapClause>(c);
        for(auto child1 : map->children()) {
          if(child1) {
            string size = "";
            string varName = "";
            if(auto array = dyn_cast<OMPArraySectionExpr>(child1)) {
              varName = getArraySize(array, size);
            } else if(dyn_cast<DeclRefExpr>(child1)) {
              varName = dyn_cast<DeclRefExpr>(child1)->getDecl()->getNameAsString();
            }

            if(enterData.back() != ' ') enterData += ", ";
            enterData += varName + size;
          }
        }
      }
    }
    enterData += ")";
  }

  return enterData;
}

string OMPAdvisorASTConsumer::getExitDataString(Kernel *k) {
  string exitData = "";
  OMPTargetDirective *omp = dyn_cast<OMPTargetDirective>(k->getStmt());
  if(omp->hasClausesOfKind<OMPMapClause>()) {
    exitData = "map(from: ";
    int numClauses = omp->getNumClauses();
    for(int i=0; i<numClauses; i++) {
      OMPClause *c = omp->getClause(i);
      if(dyn_cast<OMPMapClause>(c)) {
        OMPMapClause *map = dyn_cast<OMPMapClause>(c);
        for(auto child1 : map->children()) {
          if(child1) {
            string size = "";
            string varName = "";
            if(auto array = dyn_cast<OMPArraySectionExpr>(child1)) {
              varName = getArraySize(array, size);
            } else if(dyn_cast<DeclRefExpr>(child1)) {
              varName = dyn_cast<DeclRefExpr>(child1)->getDecl()->getNameAsString();
            }

            if(exitData.back() != ' ') exitData += ", ";
            exitData += varName + size;
          }
        }
      }
    }
    exitData += ")";
  }

  return exitData;
}

void OMPAdvisorASTConsumer::codeGen(string fileName, Kernel *k, int rID, int num_par, int par_pos, int dist_col, int par_col, int sched, int data) {
  std::ostringstream sstream;
  sstream << fileName << "_" << k->getFunction()->getNameInfo().getAsString() << "_" << k->getID() << "_" << par_pos << "_" << dist_col << "_" << par_col << "_" << sched << "_" << data <<".cpp";
  fileName = sstream.str();

  string code0;
  string code1;
  string code2;
  string code3 = "";
  string code4 = "";
  string code5 = "";
  string code6;
  string code7;

  code0 = CODE_ENTER_DATA + " " + getEnterDataString(k) + "\n";
  code1 = CODE_DEC_TIMEVAL + CODE_GETTIME_1 + CODE_START_TIME;

  code2 = CODE_DIST;
  if(dist_col > 1) code2 += " collapse(" + std::to_string(dist_col) + ")";
  if(par_pos == 2) {
    code2 += " parallel for";
    if(par_col > 1) code2 += " collapse(" + std::to_string(par_col) + ")";
    if(sched > 0) code2 += " schedule(" + string(schedule[sched]) + ")";
  }
  code2 += "\n";

  if(par_pos == 3) {
    code3 = CODE_PARALLEL_FOR;
    if(par_col > 1) code3 += " collapse(" + std::to_string(par_col) + ")";
    if(sched > 0) code3 += " schedule(" + string(schedule[sched]) + ")";
    code3 += "\n";
  }
  if(par_pos == 4) {
    code4 = CODE_PARALLEL_FOR;
    if(par_col > 1) code4 += " collapse(" + std::to_string(par_col) + ")";
    if(sched > 0) code4 += " schedule(" + string(schedule[sched]) + ")";
    code4 += "\n";
  }
  if(par_pos == 5) {
    code5 = CODE_PARALLEL_FOR;
    if(par_col > 1) code5 += " collapse(" + std::to_string(par_col) + ")";
    if(sched > 0) code5 += " schedule(" + string(schedule[sched]) + ")";
    code5 += "\n";
  }

  code6 = CODE_GETTIME_2;
  code6 += CODE_END_TIME;
  code6 += "fprintf(fp,\"";
  code6 += k->getFunction()->getNameInfo().getAsString();
  code6 += "_parPos" + std::to_string(par_pos);
  code6 += "_distCol" + std::to_string(dist_col);
  code6 += "_parCol" + std::to_string(par_col);
  code6 += "_" + string(schedule[sched]);
  code6 += (data==1 ? "_memcpy" : "" );
  code6 += ",%ld\\n\",  (end - start));\n";

  code7 = CODE_EXIT_DATA + " " + getExitDataString(k) + "\n";

  if(data == 1) {
    code0.swap(code1);
    code6.swap(code7);
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
