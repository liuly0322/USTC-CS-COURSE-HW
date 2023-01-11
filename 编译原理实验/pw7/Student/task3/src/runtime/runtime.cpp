#include "runtime/runtime.h"
#include "runtime/io.h"
#include "llvm/IR/DerivedTypes.h"
#include <iostream>

#include <llvm/IR/Constants.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Type.h>
#include <tuple>

using namespace std;
using namespace llvm;

runtime_info::runtime_info(Module *module) {
    auto *Int32Ty = Type::getInt32Ty(module->getContext());
    auto *Int32PtrTy = Type::getInt32PtrTy(module->getContext());
    auto *FloatTy = Type::getFloatTy(module->getContext());
    auto *FloatPtrTy = Type::getFloatPtrTy(module->getContext());
    auto *VoidTy = Type::getVoidTy(module->getContext());

    get_int_func = Function::Create(FunctionType::get(Int32Ty, {}, false),
                                    GlobalValue::LinkageTypes::ExternalLinkage,
                                    "getint", module);

    get_char_func = Function::Create(FunctionType::get(Int32Ty, {}, false),
                                     GlobalValue::LinkageTypes::ExternalLinkage,
                                     "getch", module);

    get_float_func = Function::Create(
        FunctionType::get(FloatTy, {}, false),
        GlobalValue::LinkageTypes::ExternalLinkage, "getfloat", module);

    get_int_array_func = Function::Create(
        FunctionType::get(Int32Ty, {Int32PtrTy}, false),
        GlobalValue::LinkageTypes::ExternalLinkage, "getarray", module);

    get_float_array_func = Function::Create(
        FunctionType::get(Int32Ty, {FloatPtrTy}, false),
        GlobalValue::LinkageTypes::ExternalLinkage, "getfarray", module);

    put_int_func = Function::Create(FunctionType::get(VoidTy, {Int32Ty}, false),
                                    GlobalValue::LinkageTypes::ExternalLinkage,
                                    "putint", module);

    put_char_func = Function::Create(
        FunctionType::get(VoidTy, {Int32Ty}, false),
        GlobalValue::LinkageTypes::ExternalLinkage, "putch", module);

    put_float_func = Function::Create(
        FunctionType::get(VoidTy, {FloatTy}, false),
        GlobalValue::LinkageTypes::ExternalLinkage, "putfloat", module);

    put_int_array_func = Function::Create(
        FunctionType::get(VoidTy, {Int32Ty, Int32PtrTy}, false),
        GlobalValue::LinkageTypes::ExternalLinkage, "putarray", module);

    put_float_array_func = Function::Create(
        FunctionType::get(VoidTy, {Int32Ty, FloatPtrTy}, false),
        GlobalValue::LinkageTypes::ExternalLinkage, "putfarray", module);

    current_time_func = Function::Create(
        FunctionType::get(Int32Ty, {}, false),
        GlobalValue::LinkageTypes::ExternalLinkage, "current_time", module);
}

using namespace string_literals;

std::vector<std::tuple<std::string, void *>>
runtime_info::get_runtime_symbols() {
    return {
        make_tuple("getint"s, reinterpret_cast<void *>(&::getint)),
        make_tuple("getchar"s, reinterpret_cast<void *>(&::getch)),
        make_tuple("getfloat"s, reinterpret_cast<void *>(&::getfloat)),
        make_tuple("getarray"s, reinterpret_cast<void *>(&::getarray)),
        make_tuple("getfarray"s, reinterpret_cast<void *>(&::getfarray)),
        make_tuple("putint"s, reinterpret_cast<void *>(&::putint)),
        make_tuple("putch"s, reinterpret_cast<void *>(&::putch)),
        make_tuple("putfloat"s, reinterpret_cast<void *>(&::putfloat)),
        make_tuple("putarray"s, reinterpret_cast<void *>(&::putarray)),
        make_tuple("putfarray"s, reinterpret_cast<void *>(&::putfarray)),
        make_tuple("current_time"s, reinterpret_cast<void *>(&::current_time)),
    };
}