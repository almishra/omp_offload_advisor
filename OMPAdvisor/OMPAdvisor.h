#ifndef _OMP_ADVISOR_H_
#define _OMP_ADVISOR_H_

#define DEBUG false

class Loop {
    const int id;
    clang::Stmt *st;
    std::set<int> kernels;

    std::set<clang::VarDecl*> privList;
    std::set<clang::ValueDecl*> valIn;
    std::set<clang::ValueDecl*> valOut;
    std::set<clang::ValueDecl*> valInOut;

    clang::SourceLocation startLoc;
    clang::SourceLocation endLoc;

public:
    Loop(int ID, clang::Stmt *stmt) : id(ID), st(stmt) {};

    /* Get ID for this loop. Every loop has a unique ID number which is
     * set in the constructor */
    int getID() const;

    /* Get the Stmt of the loop*/
    clang::Stmt *getStmt();

    /* Add kernel ID for kernels inside the loop */
    void addKernel(int k_id);

    /* Get list of all kernels inside the loop */
    std::set<int> getKernels();

    /* Add/Get/Remove private variables for this loop */
    void addPrivate(clang::VarDecl *d);
    std::set<clang::VarDecl*> getPrivate();
    void removePrivate(clang::VarDecl *d);

    /* Add/Get/Remove variables which need to be transferred to the device 
     * for this loop */
    void addValueIn(clang::ValueDecl *d);
    std::set<clang::ValueDecl*> getValIn();
    void removeValueIn(clang::ValueDecl *d);

    /* Add/Get/Remove variables which need to be transferred from the device 
     * for this loop */
    void addValueOut(clang::ValueDecl *d);
    std::set<clang::ValueDecl*> getValOut();
    void removeValueOut(clang::ValueDecl *d);

    /* Add/Get/Remove variables which need to be transferred both to and from 
     * the device for this loop */
    void addValueInOut(clang::ValueDecl *d);
    std::set<clang::ValueDecl*> getValInOut();
    void removeValueInOut(clang::ValueDecl *d);

    /* Getter/Setter for start and end source location of the loop */
    void setStartLoc(clang::SourceLocation start);
    clang::SourceLocation getStartLoc();
    void setEndLoc(clang::SourceLocation end);
    clang::SourceLocation getEndLoc();

    // Overloading operator < and > for sorting of keys in map
    bool operator< (const Loop& l) const;
    bool operator> (const Loop& l) const;
};


/************************************************************/
class Kernel {
    const int id;
    clang::Stmt *st;
    bool inLoop;
    Loop *loop;
    clang::FunctionDecl *FD;
    int numNestedLoop;

    int link;

    clang::SourceLocation codeLoc[8]; // The 7 potential code location. 8th location is for error.

    std::set<clang::VarDecl*> privList;
    std::set<clang::ValueDecl*> valIn;
    std::set<clang::ValueDecl*> valOut;
    std::set<clang::ValueDecl*> valInOut;

    std::set<clang::ValueDecl*> sharedValIn;
    std::set<clang::ValueDecl*> sharedValOut;
    std::set<clang::ValueDecl*> sharedValInOut;

    clang::SourceLocation startLoc;
    clang::SourceLocation endLoc;
    std::string getEnterDataString(Kernel *k);
    std::string getExitDataString(Kernel *k);

public:
    Kernel(int ID, clang::Stmt *stmt, clang::FunctionDecl *F);

    /* Get ID for this kernel. Every kernel has a unique ID number which is
     * set in the constructor */
    int getID() const;

    /* Getter/Setter of number of Collapsible loops */
    int getNumNestedLoop();
    void findNestedLoops();

    /* Getter/Setter of location for generated code */
    clang::SourceLocation getCodeLocation(int N);
    void setCodeLocation(int N, clang::SourceLocation Loc);

    /* Getter/Setter that kernel is inside a loop */
    bool isInLoop();
    void setInLoop(bool in);

    /* Getter/Setter for the Stmt of the loop this kernel is inside */
    Loop *getLoop();
    void setLoop(Loop *l);

    /* Getter/Setter for the function from which this kernel is called */
    clang::FunctionDecl *getFunction();
    void setFuction(clang::FunctionDecl *F);

    /* Get the Stmt of the kernel */
    clang::Stmt *getStmt();

    /* Add/Get/Remove private variables for this kernel */
    void addPrivate(clang::VarDecl *d);
    std::set<clang::VarDecl*> getPrivate();
    void removePrivate(clang::VarDecl *d);

    /* Add/Get/Remove variables which need to be transferred to the device 
     * for this kernel */
    void addValueIn(clang::ValueDecl *d);
    std::set<clang::ValueDecl*> getValIn();
    void removeValueIn(clang::ValueDecl *d);

    /* Add/Get/Remove variables which need to be transferred from the device 
     * for this kernel */
    void addValueOut(clang::ValueDecl *d);
    std::set<clang::ValueDecl*> getValOut();
    void removeValueOut(clang::ValueDecl *d);

    /* Add/Get/Remove variables which need to be transferred both to and from 
     * the device for this kernel */
    void addValueInOut(clang::ValueDecl *d);
    std::set<clang::ValueDecl*> getValInOut();
    void removeValueInOut(clang::ValueDecl *d);

    /* Add/Get/Remove variables which need to be transferred to the device 
     * and is shared between this kernel and some other kernel */
    void addSharedValueIn(clang::ValueDecl *d);
    std::set<clang::ValueDecl*> getSharedValIn();
    void removeSharedValueIn(clang::ValueDecl *d);

    /* Add/Get/Remove variables which need to be transferred from the device 
     * and is shared between this kernel and some other kernel */
    void addSharedValueOut(clang::ValueDecl *d);
    void removeSharedValueOut(clang::ValueDecl *d);
    std::set<clang::ValueDecl*> getSharedValOut();
 
    /* Add/Get/Remove variables which need to be transferred both to and from 
     * the device and is shared between this kernel and some other kernel */
    void addSharedValueInOut(clang::ValueDecl *d);
    void removeSharedValueInOut(clang::ValueDecl *d);
    std::set<clang::ValueDecl*> getSharedValInOut();

    /* Getter/Setter for start and end source location of the kernel */
    void setStartLoc(clang::SourceLocation start);
    clang::SourceLocation getStartLoc();
    void setEndLoc(clang::SourceLocation end);
    clang::SourceLocation getEndLoc();

    void setLink(int id);
    int isLinkedTo();

    // Overloading operator < and > for sorting of keys in map
    bool operator< (const Kernel& k) const;
    bool operator> (const Kernel& k) const;
};

/************************************************************/
class OMPAdvisorVisitor : public clang::RecursiveASTVisitor<OMPAdvisorVisitor> {
  private:
    clang::ASTContext *astContext;
    clang::SourceManager *SM;
    std::map<int, std::vector<Kernel*>> kernel_map;
    std::map<int, std::vector<Loop*>> loop_map;

    clang::BeforeThanCompare<clang::SourceLocation> isBefore;

    clang::FunctionDecl *currentFunction;
    Kernel *lastKernel;
    clang::SourceLocation loopEnd;
    Loop *lastLoop;

    bool insideLoop;
    bool insideKernel;
    int num_collapse;

    bool firstPrivate;

    /* Check is the given SourceLocation is within the last known kernel */     
    bool isWithin(clang::SourceLocation src);

    /* Check if the given variable is a private variable of the last
     * known kernel */
    bool isNotPrivate(clang::ValueDecl* v);

    /* Get the leftmost node of an Operator Stmt */
    clang::ValueDecl *getLeftmostNode(clang::Stmt *st);

    /* Template function to check the type of the Stmt */
    template <typename T>
    bool kernel_found(clang::Stmt *st) {
      auto cast_value = llvm::dyn_cast<T>(st);
      if (cast_value != nullptr) return true;

      return false;
    }

  public:
    explicit OMPAdvisorVisitor(clang::CompilerInstance *CI);

    std::map<int, std::vector<Kernel*>> getKernelMap();
    std::map<int, std::vector<Loop*>> getLoopMap();     

    /* Visitor fucntions for RecursiveASTVisitor */
    virtual bool VisitFunctionDecl(clang::FunctionDecl *FD);
    virtual bool VisitStmt(clang::Stmt *st);
};


/************************************************************/
class OMPAdvisorASTConsumer : public clang::ASTConsumer {
  private:
    OMPAdvisorVisitor *visitor;
    clang::CompilerInstance *ci;
    clang::Rewriter rewriter[84];
    clang::SourceManager *SM;
    clang::ASTContext *astContext;
    void getKernelInfo();
    void getLoopInfo();

    std::map<int, std::vector<Kernel*>> kernel_map;
    std::map<int, std::vector<Loop*>> loop_map; 

    struct MatchPathSeparator {                                                 
      bool operator()(char ch) const { return ch == '/'; }                    
    };
    void codeGen(std::string fileName, Kernel *k, int rID, int num_par, int par_pos,
                 int dist_col, int par_col, int sched, int data);
    std::string getArraySize(clang::OMPArraySectionExpr *array, std::string &size);
    std::string getEnterDataString(Kernel *k);
    std::string getExitDataString(Kernel *k);

  public:
    OMPAdvisorASTConsumer(clang::CompilerInstance *CI);
    void HandleTranslationUnit(clang::ASTContext &Context);
    std::string basename(std::string path);
};

/************************************************************/
class PluginOMPAdvisor : public clang::PluginASTAction {
  protected:
    std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance &CI, llvm::StringRef file);
    bool ParseArgs(const clang::CompilerInstance &CI, const std::vector<std::string> &args);
    void PrintHelp(llvm::raw_ostream& os);
};

#endif // End _OMP_ADVISOR_H_
