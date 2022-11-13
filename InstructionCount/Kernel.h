#ifndef INSTRUCTIONCOUNT_KERNEL_H
#define INSTRUCTIONCOUNT_KERNEL_H

class Kernel {
  int id;
  bool inLoop;
  clang::Stmt *st;
  clang::Stmt *loop;
  clang::FunctionDecl *FD;

  //enum TYPE { I8, I16, I32, I64, F32, F64, NONE, TYPE_COUNT };
  //const std::string STR[TYPE_COUNT] = { "i8", "i16", "i32", "i64", "f32", "f64", "NONE" };
  enum TYPE { I32, I64, F32, F64, NONE, TYPE_COUNT };
  const std::string STR[TYPE_COUNT] = { "i32", "i64", "f32", "f64", "NONE" };

  long numIteration;

  long varDecl;
  long refExpr;
  long intLiteral;
  long floatLiteral;
  long fpLiteral;
  long charLiteral;
  long funcCall;

  long long mem_to;
  long long mem_from;

  long add_sub[TYPE_COUNT];
  long mul[TYPE_COUNT];
  long div[TYPE_COUNT];
  long logical[TYPE_COUNT];
  long rel[TYPE_COUNT];
  long assign[TYPE_COUNT];
  long bit[TYPE_COUNT];
  int rem[TYPE_COUNT];

  public:
  Kernel(int ID, clang::Stmt *stmt, clang::FunctionDecl *F) : id(ID), st(stmt), FD(F) {
    inLoop = false;
    loop = NULL;

    numIteration = 0;

    varDecl = 0;
    refExpr = 0;
    intLiteral = 0;
    floatLiteral = 0;
    fpLiteral = 0;
    charLiteral = 0;
    funcCall = 0;

    mem_to = 0;
    mem_from = 0;

    for(int i=0; i<TYPE_COUNT; i++) {
      add_sub[i] = 0;
      mul[i] = 0;
      div[i] = 0;
      logical[i] = 0;
      rel[i] = 0;
      assign[i] = 0;
      bit[i] = 0;
      rem[i] = 0;
    }
  }

  int getID() { return id; }
  void setInLoop(bool in) { inLoop = in; }
  bool isInLoop() { return inLoop; }
  clang::Stmt *getStmt() { return st; }
  clang::Stmt *getLoop() { return loop; }
  void setLoop(clang::Stmt *l) { loop = l; }
  clang::FunctionDecl *getFunction() { return FD; }
  void setFuction(clang::FunctionDecl *F) { FD = F; }

  void setNumIteration(long num) { numIteration = num; }
  long getNumIteration() { return numIteration; }

  void setMemTo(long num) { mem_to = num; }
  long getMemTo() { return mem_to; }

  void setMemFrom(long num) { mem_from = num; }
  long getMemFrom() { return mem_from; }

  void incrStmt(clang::Stmt *st, int count) {
    llvm::errs().changeColor(llvm::raw_ostream::YELLOW);
    llvm::errs().resetColor();
    TYPE type = NONE;

    //TODO : Currently only considering Builtin types. Add cases for other types
    if(llvm::dyn_cast<clang::BinaryOperator>(st) || 
       llvm::dyn_cast<clang::UnaryOperator>(st)) {
    const clang::BuiltinType *T;
    if(auto b = llvm::dyn_cast<clang::BinaryOperator>(st))
      T = llvm::dyn_cast<clang::BuiltinType>(b->getType());
    if(auto u = llvm::dyn_cast<clang::UnaryOperator>(st))
      T = llvm::dyn_cast<clang::BuiltinType>(u->getType());
    auto k = T->getKind();
    switch (T->getKind()) {
      case clang::BuiltinType::Void:
      case clang::BuiltinType::Bool:
      case clang::BuiltinType::Char_S:
      case clang::BuiltinType::Char_U:
      case clang::BuiltinType::Char8:
      case clang::BuiltinType::UChar:
      case clang::BuiltinType::SChar:
       // type = I8;
        break;
      case clang::BuiltinType::Char16:
      case clang::BuiltinType::UShort:
      case clang::BuiltinType::Short:
       // type = I16;
        break;
      case clang::BuiltinType::WChar_S:
      case clang::BuiltinType::WChar_U:
      case clang::BuiltinType::Char32:
      case clang::BuiltinType::UInt:
      case clang::BuiltinType::Int:
        type = I32;
        break;
      case clang::BuiltinType::ULongLong:
      case clang::BuiltinType::LongLong:
      case clang::BuiltinType::ULong:
      case clang::BuiltinType::Long:
        type = I64;
        break;
      case clang::BuiltinType::Float:
        type = F32;
        break;
      case clang::BuiltinType::Double:
        type = F64;
        break;
      case clang::BuiltinType::Int128:
      case clang::BuiltinType::UInt128:
      case clang::BuiltinType::ShortAccum:
      case clang::BuiltinType::UShortAccum:
      case clang::BuiltinType::SatShortAccum:
      case clang::BuiltinType::SatUShortAccum:
      case clang::BuiltinType::Accum:
      case clang::BuiltinType::UAccum:
      case clang::BuiltinType::SatAccum:
      case clang::BuiltinType::SatUAccum:
      case clang::BuiltinType::LongAccum:
      case clang::BuiltinType::ULongAccum:
      case clang::BuiltinType::SatLongAccum:
      case clang::BuiltinType::SatULongAccum:
      case clang::BuiltinType::ShortFract:
      case clang::BuiltinType::UShortFract:
      case clang::BuiltinType::SatShortFract:
      case clang::BuiltinType::SatUShortFract:
      case clang::BuiltinType::Fract:
      case clang::BuiltinType::UFract:
      case clang::BuiltinType::SatFract:
      case clang::BuiltinType::SatUFract:
      case clang::BuiltinType::LongFract:
      case clang::BuiltinType::ULongFract:
      case clang::BuiltinType::SatLongFract:
      case clang::BuiltinType::SatULongFract:
      case clang::BuiltinType::BFloat16:
      case clang::BuiltinType::Float16:
      case clang::BuiltinType::Half:
      case clang::BuiltinType::LongDouble:
      case clang::BuiltinType::Float128:
      case clang::BuiltinType::NullPtr:
      case clang::BuiltinType::ObjCId:
      case clang::BuiltinType::ObjCClass:
      case clang::BuiltinType::ObjCSel:
      case clang::BuiltinType::OCLSampler:
      case clang::BuiltinType::OCLEvent:
      case clang::BuiltinType::OCLClkEvent:
      case clang::BuiltinType::OCLQueue:
      case clang::BuiltinType::OCLReserveID:
      default:
        llvm::errs().changeColor(llvm::raw_ostream::YELLOW);
        llvm::errs().resetColor();
        break;
    }
    }
    llvm::errs().changeColor(llvm::raw_ostream::YELLOW);
    llvm::errs().resetColor();
    switch(st->getStmtClass()) {
      case clang::Stmt::CompoundAssignOperatorClass:
      case clang::Stmt::BinaryOperatorClass: {
        clang::BinaryOperator *bin = llvm::dyn_cast<clang::BinaryOperator>(st);
        clang::BinaryOperatorKind o = bin->getOpcode();
        switch(o) {
          case clang::BO_Mul: mul[type]+=count; break;
          case clang::BO_Div: div[type]+=count; break;
          case clang::BO_Rem: rem[type]+=count; break;
          case clang::BO_Add: add_sub[type]+=count; break;
          case clang::BO_Sub: add_sub[type]+=count; break;
          case clang::BO_LT: rel[type]+=count; break;
          case clang::BO_LE: rel[type]+=count; break;
          case clang::BO_GT: rel[type]+=count; break;
          case clang::BO_GE: rel[type]+=count; break;
          case clang::BO_EQ: rel[type]+=count; break;
          case clang::BO_NE: rel[type]+=count; break;
          case clang::BO_And: bit[type]+=count; break;
          case clang::BO_Xor: bit[type]+=count; break;
          case clang::BO_Or: bit[type]+=count; break;
          case clang::BO_LAnd: logical[type]+=count; break;
          case clang::BO_LOr: logical[type]+=count; break;
          case clang::BO_Assign: assign[type]+=count; break;
          case clang::BO_MulAssign: mul[type]+=count; assign[type]+=count; break;
          case clang::BO_DivAssign: div[type]+=count; assign[type]+=count; break;
          case clang::BO_RemAssign: rem[type]+=count; assign[type]+=count; break;
          case clang::BO_AddAssign: add_sub[type]+=count; assign[type]+=count; break;
          case clang::BO_SubAssign: add_sub[type]+=count; assign[type]+=count; break;
          case clang::BO_AndAssign: bit[type]+=count; assign[type]+=count; break;
          case clang::BO_XorAssign: bit[type]+=count; assign[type]+=count; break;
          case clang::BO_OrAssign: bit[type]+=count; assign[type]+=count; break;
          default:  break;
        }
        break;
      }
      case clang::Stmt::UnaryOperatorClass: {
        clang::UnaryOperator *un = llvm::dyn_cast<clang::UnaryOperator>(st);
        clang::UnaryOperatorKind o = un->getOpcode();
        switch(o) {
          case clang::UO_PostInc: add_sub[type]+=count; break;
          case clang::UO_PostDec: add_sub[type]+=count; break;
          case clang::UO_PreInc: add_sub[type]+=count; break;
          case clang::UO_PreDec: add_sub[type]+=count; break;
          case clang::UO_Plus: bit[type]+=count; break;
          case clang::UO_Minus: bit[type]+=count; break;
          case clang::UO_Not: bit[type]+=count; break;
          case clang::UO_LNot: logical[type]+=count; break;
          default: break;
        }
        break;
      }
      case clang::Stmt::IntegerLiteralClass: intLiteral+=count;
        break;
      case clang::Stmt::FloatingLiteralClass: floatLiteral+=count; break;
      case clang::Stmt::FixedPointLiteralClass: fpLiteral+=count; break;
      case clang::Stmt::CharacterLiteralClass: charLiteral+=count; break;
      case clang::Stmt::CallExprClass: funcCall+=count; break;
      case clang::Stmt::DeclRefExprClass: refExpr+=count; break;
      case clang::Stmt::DeclStmtClass: varDecl+=count; break;
      default: break;
    }
  }

  void dump() {
    llvm::outs() << FD->getNameInfo().getAsString() << ",";
    llvm::outs() << numIteration << ",";
    llvm::outs() << varDecl << ",";
    llvm::outs() << refExpr << ",";
    llvm::outs() << intLiteral << ",";
    llvm::outs() << floatLiteral << ",";

    llvm::outs() << mem_to << ",";
    llvm::outs() << mem_from << ",";

    for(int i=0; i<NONE; i++) llvm::outs() << add_sub[i] << ",";
    for(int i=0; i<NONE; i++) llvm::outs() << mul[i] << ",";
    for(int i=0; i<NONE; i++) llvm::outs() << div[i] << ",";
    for(int i=0; i<NONE; i++) llvm::outs() << logical[i] << ",";
    for(int i=0; i<NONE; i++) llvm::outs() << rem[i] << ",";
    for(int i=0; i<NONE; i++) llvm::outs() << assign[i] << ",";
    llvm::outs() << "\n";
  }

  void print() {
    llvm::errs() << "Total Number of Iterations: " << numIteration << "\n\n";

    llvm::errs() << "varDecl : " << varDecl << "\n";
    llvm::errs() << "refExpr  : " << refExpr << "\n";
    llvm::errs() << "intLiteral  : " << intLiteral << "\n";
    llvm::errs() << "floatLiteral  : " << floatLiteral << "\n";
    llvm::errs() << "fpLiteral  : " << fpLiteral << "\n";
    llvm::errs() << "charLiteral  : " << charLiteral << "\n";
    llvm::errs() << "funcCall  : " << funcCall << "\n\n";

    for(int i=0; i<NONE; i++) llvm::outs() << add_sub[i] << ",";
    for(int i=0; i<NONE; i++) llvm::outs() << mul[i] << ",";
    for(int i=0; i<NONE; i++) llvm::outs() << div[i] << ",";
    for(int i=0; i<NONE; i++) llvm::outs() << bit[i] << ",";
    for(int i=0; i<NONE; i++) llvm::outs() << rel[i] << ",";
    for(int i=0; i<NONE; i++) llvm::outs() << logical[i] << ",";
    for(int i=0; i<NONE; i++) llvm::outs() << assign[i] << ",";

    for(int i=0; i<NONE; i++)
      llvm::errs() << "add_sub_" << STR[i] << "  : " << add_sub[i] << "\n";

    for(int i=0; i<NONE; i++)
      llvm::errs() << "mul_" << STR[i] << "  : " << mul[i] << "\n";

    for(int i=0; i<NONE; i++)
      llvm::errs() << "div_" << STR[i] << "  : " << div[i] << "\n";

    for(int i=0; i<NONE; i++)
      llvm::errs() << "bit_" << STR[i] << "  : " << bit[i] << "\n";

    for(int i=0; i<NONE; i++)
      llvm::errs() << "rel_" << STR[i] << "  : " << rel[i] << "\n";

    for(int i=0; i<NONE; i++)
      llvm::errs() << "logical_" << STR[i] << "  : " << logical[i] << "\n";

    for(int i=0; i<NONE; i++)
      llvm::errs() << "assign_" << STR[i] << "  : " << assign[i] << "\n";
  }

  // Overloading operator < for sorting of keys in map
  bool operator< (const Kernel& k) const {
    if(k.id < this->id) return true;
    return false;
  }
};

#endif // End of INSTRUCTIONCOUNT_KERNEL_H
