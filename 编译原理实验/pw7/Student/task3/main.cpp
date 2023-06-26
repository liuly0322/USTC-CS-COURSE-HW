#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>

#include "clang/Frontend/TextDiagnosticPrinter.h"
#include "clang/Basic/TargetInfo.h"
#include "clang/Basic/DiagnosticFrontend.h"

#include "llvm/ADT/SmallString.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Mangler.h"
#include "llvm/Support/ManagedStatic.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Transforms/Utils.h"

#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/Support/DynamicLibrary.h>
#include <llvm/Support/TargetSelect.h>

#include "Driver/driver.h"
#include "IR/gcd_generator.hpp"
#include "IR/qs_generator.hpp"


using namespace llvm;
using namespace clang;
using namespace mDriver;
using namespace std;

int main(int argc, const char **argv) {

  // 创建诊断函数
  IntrusiveRefCntPtr<DiagnosticOptions> DiagOpts = new DiagnosticOptions();
  TextDiagnosticPrinter *DiagClient =
    new TextDiagnosticPrinter(llvm::errs(), &*DiagOpts);

  IntrusiveRefCntPtr<DiagnosticIDs> DiagID(new DiagnosticIDs());
  DiagnosticsEngine Diags(DiagID, &*DiagOpts, DiagClient);

  // 得到当前进程运行所在的目标平台信息，用于 JIT 中把代码 load 进当前进程
  const std::string TripleStr = llvm::sys::getProcessTriple();
  llvm::Triple T(TripleStr);

  // 调用 Driver，实例化
  llvm::ErrorOr<std::string> ClangPath = llvm::sys::findProgramByName("clang");
  if (!ClangPath) {
      llvm::errs() << "clang not found.\n";
      exit(1);
  }
  Driver TheDriver(StringRef((*ClangPath).c_str()), T.str(), Diags);
  SmallVector<const char *, 16> Args(argv, argv + argc);
  Args.push_back("-fsyntax-only");
  if(TheDriver.ParseArgs(Args) && TheDriver.BuildCI(Diags)){
    if(TheDriver.is_verbose) 
    {
      llvm::outs() << "Use clang: " <<  (*ClangPath).c_str() << "\n";
    }
  }
  else{
    exit(1);
  }

  LLVMContext* context = new LLVMContext();
  llvm::Module* module;
  runtime_info* runtime;

  //输入文件为源文件
  if(!TheDriver.is_IR){
      TheDriver.printASTUnit();
      module = TheDriver.FrontendCodeGen(nullptr);
      runtime = new runtime_info(module);
  }
  else{
      // 直接调用 IR 构造函数，获得 llvm::Module
      // module = gcd_generator(*context, runtime);
      /**
       * @brief TODO
       * 注释上一行代码，并在此处调用 quicksort.hpp 中的 qs_generator 获得 IR Module
       */
      module = qs_generator(*context, runtime);

      module = TheDriver.FrontendCodeGen(module);
  }
  
  TheDriver.InitializePasses();
  TheDriver.addPass(createPromoteMemoryToRegisterPass());
  TheDriver.run();

  if (!module)
  {
      llvm::errs() << "Semantic failed. Exiting.\n";
      return 3;
  }

  auto entry_func = module->getFunction("main");
  if (!entry_func)
  {
      llvm::errs() << "No 'main' function presented. Exiting.\n";
      return 4;
  }
  InitializeNativeTargetAsmParser();
  for (auto t : runtime->get_runtime_symbols())
    sys::DynamicLibrary::AddSymbol(get<0>(t), get<1>(t));

  std::string error_info;
  ExecutionEngine* engine = EngineBuilder(std::move(TheDriver.get_module()))
                                          .setEngineKind(EngineKind::JIT)
                                          .setErrorStr(&error_info)
                                          .create();

  if (!engine)
  {
      llvm::errs() << "EngineBuilder failed: " << error_info << "\n";
      return 4;
  }
  engine->runFunction(entry_func, {});

  // Shutdown.
  llvm::llvm_shutdown();
  return 0;
}
