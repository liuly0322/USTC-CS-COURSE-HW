#ifndef M_DRIVER_H
#define M_DRIVER_H
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Driver/Driver.h"
#include "clang/Driver/Compilation.h"
#include "clang/CodeGen/CodeGenAction.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/StaticAnalyzer/Frontend/FrontendActions.h"


#include "llvm/IR/Module.h"
#include "llvm/Pass.h"
#include "llvm/IR/LegacyPassManager.h"
#include <string>
using namespace llvm;
using namespace clang;

namespace mDriver {
using cDriver = clang::driver::Driver;
using SVec = SmallVector<const char *, 16>;

// Driver接收单文件，解析并产生AST和LLVM IR的Module
class Driver {
private:
    cDriver _TheDriver;
    bool _show_ir_after_pass = false;
    bool _show_AST = false;
    std::string _OutFile = "";
    SVec _Args;
    std::unique_ptr<driver::Compilation> _C;
    clang::CompilerInstance _Clang;
    std::unique_ptr<CodeGenAction> _Act;
    // 保存Clang 代码生成的LLVM IR的Module
    std::unique_ptr<llvm::Module> _M;
    size_t _PassID = 0;
    PassRegistry* _PassRegistry;
    std::unique_ptr<legacy::PassManager> _PM;
    std::unique_ptr<legacy::FunctionPassManager> _FPM;

public:
    Driver(StringRef ClangExecutable, StringRef TargetTriple,
         DiagnosticsEngine &Diags);
    ~Driver() { _Args.clear(); }
    virtual bool ParseArgs(SVec &);
    bool BuildCI(DiagnosticsEngine &);
    llvm::Module* FrontendCodeGen(llvm::Module *);
    bool runonFunction();
    bool runonmodule();

    void printASTUnit();
    void InitializePasses();
    void addPass(FunctionPass*);
    void addPass(ModulePass*);
    void run();
    bool is_IR = false;
    bool is_verbose = false;
    std::unique_ptr<llvm::Module> get_module();
};


}


#endif // M_DRIVER_H