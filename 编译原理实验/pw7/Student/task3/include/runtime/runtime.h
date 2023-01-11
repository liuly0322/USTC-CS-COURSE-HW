
#ifndef _C1_RUNTIME_H_
#define _C1_RUNTIME_H_

#include <vector>
#include <tuple>

#include <llvm/IR/Function.h>
#include <llvm/IR/Module.h>

class runtime_info
{
public:
    llvm::Function *get_int_func;
    llvm::Function *get_char_func;
    llvm::Function *get_float_func;
    llvm::Function *get_int_array_func;
    llvm::Function *get_float_array_func;
    llvm::Function *put_int_func;
    llvm::Function *put_char_func;
    llvm::Function *put_float_func;
    llvm::Function *put_int_array_func;
    llvm::Function *put_float_array_func;
    llvm::Function *current_time_func;

    runtime_info(llvm::Module *module);
    std::vector<std::tuple<std::string, void *>> get_runtime_symbols();
};

#endif