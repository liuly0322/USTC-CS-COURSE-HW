#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>

#include "runtime/runtime.h"
#include "llvm/IR/GlobalValue.h"
#include <iostream>
#include <memory>
#include <string>
#include <vector>

using namespace llvm; // 指明命名空间为llvm

#define CONST(num) ConstantInt::get(context, APInt(32, num))

llvm::Module *qs_generator(LLVMContext &context, runtime_info *&runtime) {
    IRBuilder<> builder(context);
    auto *module = new llvm::Module("quick_sort", context);
    runtime = new runtime_info(module);

    llvm::Type *IntType = llvm::Type::getInt32Ty(context);
    llvm::Type *Int8PtrType = llvm::Type::getInt8PtrTy(context);
    llvm::Type *FloatType = llvm::Type::getFloatTy(context);
    llvm::Type *FloatPtrType = llvm::Type::getFloatPtrTy(context);
    llvm::Type *VoidType = llvm::Type::getVoidTy(context);

    auto *buf_size = module->getOrInsertGlobal("buf_size", IntType);
    module->getNamedGlobal("buf_size")->setInitializer(CONST(4096));

    // quickSort 函数
    auto *quickSortFun = Function::Create(
        llvm::FunctionType::get(VoidType, {FloatPtrType, IntType, IntType},
                                false),
        GlobalValue::LinkageTypes::ExternalLinkage, "quickSort", module);
    auto *quickSortEntryBB = BasicBlock::Create(context, "entry", quickSortFun);
    auto *quickSortTrueBB = BasicBlock::Create(context, "true", quickSortFun);
    auto *quickSortWhileCondBB =
        BasicBlock::Create(context, "while_cond", quickSortFun);
    std::vector<BasicBlock *> quickSortWhileBodyBBs(10);
    for (auto i = 0; i < 10; i++) {
        quickSortWhileBodyBBs[i] = BasicBlock::Create(
            context, "while_body" + std::to_string(i), quickSortFun);
    }
    auto *quickSortWhileExitBB =
        BasicBlock::Create(context, "while_exit", quickSortFun);
    auto *quickSortExitBB = BasicBlock::Create(context, "exit", quickSortFun);
    builder.SetInsertPoint(quickSortEntryBB);
    // 判断 low < high 进入 TrueBB
    auto *riter = quickSortFun->arg_end();
    auto *high = --riter;
    auto *low = --riter;
    auto *arr = --riter;
    auto *low_lt_high = builder.CreateICmpSLT(low, high);
    builder.CreateCondBr(low_lt_high, quickSortTrueBB, quickSortExitBB);
    builder.ClearInsertionPoint();
    // TrueBB
    builder.SetInsertPoint(quickSortTrueBB);
    auto *iAlloca = builder.CreateAlloca(IntType);
    auto *jAlloca = builder.CreateAlloca(IntType);
    builder.CreateStore(low, iAlloca);
    builder.CreateStore(high, jAlloca);
    auto *kPointer = builder.CreateGEP(arr, low);
    auto *k = builder.CreateLoad(kPointer);
    builder.CreateBr(quickSortWhileCondBB);
    builder.ClearInsertionPoint();
    // WhileCondBB
    builder.SetInsertPoint(quickSortWhileCondBB);
    auto *i = builder.CreateLoad(iAlloca);
    auto *j = builder.CreateLoad(jAlloca);
    auto *i_lt_j = builder.CreateICmpSLT(i, j);
    builder.CreateCondBr(i_lt_j, quickSortWhileBodyBBs[0],
                         quickSortWhileExitBB);
    builder.ClearInsertionPoint();
    // WhileBodyBB[0]
    builder.SetInsertPoint(quickSortWhileBodyBBs[0]);
    i = builder.CreateLoad(iAlloca);
    j = builder.CreateLoad(jAlloca);
    i_lt_j = builder.CreateICmpSLT(i, j);
    builder.CreateCondBr(i_lt_j, quickSortWhileBodyBBs[1],
                         quickSortWhileBodyBBs[3]);
    builder.ClearInsertionPoint();
    // WhileBodyBB[1]
    builder.SetInsertPoint(quickSortWhileBodyBBs[1]);
    j = builder.CreateLoad(jAlloca);
    auto *arr_j_pointer = builder.CreateGEP(arr, j);
    auto *arr_j = builder.CreateLoad(arr_j_pointer);
    auto *arr_j_ge_k = builder.CreateFCmpOGE(arr_j, k);
    builder.CreateCondBr(arr_j_ge_k, quickSortWhileBodyBBs[2],
                         quickSortWhileBodyBBs[3]);
    builder.ClearInsertionPoint();
    // WhileBodyBB[2]
    builder.SetInsertPoint(quickSortWhileBodyBBs[2]);
    j = builder.CreateLoad(jAlloca);
    auto *j_minus_1 = builder.CreateSub(j, CONST(1));
    builder.CreateStore(j_minus_1, jAlloca);
    builder.CreateBr(quickSortWhileBodyBBs[0]);
    builder.ClearInsertionPoint();
    // WhileBodyBB[3]
    builder.SetInsertPoint(quickSortWhileBodyBBs[3]);
    i = builder.CreateLoad(iAlloca);
    j = builder.CreateLoad(jAlloca);
    i_lt_j = builder.CreateICmpSLT(i, j);
    builder.CreateCondBr(i_lt_j, quickSortWhileBodyBBs[4],
                         quickSortWhileBodyBBs[5]);
    builder.ClearInsertionPoint();
    // WhileBodyBB[4], arr[i++] = arr[j];
    builder.SetInsertPoint(quickSortWhileBodyBBs[4]);
    i = builder.CreateLoad(iAlloca);
    j = builder.CreateLoad(jAlloca);
    arr_j_pointer = builder.CreateGEP(arr, j);
    arr_j = builder.CreateLoad(arr_j_pointer);
    auto *arr_i_pointer = builder.CreateGEP(arr, i);
    builder.CreateStore(arr_j, arr_i_pointer);
    auto *i_plus_1 = builder.CreateAdd(i, CONST(1));
    builder.CreateStore(i_plus_1, iAlloca);
    builder.CreateBr(quickSortWhileBodyBBs[5]);
    builder.ClearInsertionPoint();
    // WhileBodyBB[5], i < j?
    builder.SetInsertPoint(quickSortWhileBodyBBs[5]);
    i = builder.CreateLoad(iAlloca);
    j = builder.CreateLoad(jAlloca);
    i_lt_j = builder.CreateICmpSLT(i, j);
    builder.CreateCondBr(i_lt_j, quickSortWhileBodyBBs[6],
                         quickSortWhileBodyBBs[8]);
    builder.ClearInsertionPoint();
    // WhileBodyBB[6], arr[i] < k?
    builder.SetInsertPoint(quickSortWhileBodyBBs[6]);
    i = builder.CreateLoad(iAlloca);
    arr_i_pointer = builder.CreateGEP(arr, i);
    auto *arr_i = builder.CreateLoad(arr_i_pointer);
    auto *arr_i_lt_k = builder.CreateFCmpOLT(arr_i, k);
    builder.CreateCondBr(arr_i_lt_k, quickSortWhileBodyBBs[7],
                         quickSortWhileBodyBBs[8]);
    builder.ClearInsertionPoint();
    // WhileBodyBB[7], i++
    builder.SetInsertPoint(quickSortWhileBodyBBs[7]);
    i = builder.CreateLoad(iAlloca);
    i_plus_1 = builder.CreateAdd(i, CONST(1));
    builder.CreateStore(i_plus_1, iAlloca);
    builder.CreateBr(quickSortWhileBodyBBs[5]);
    builder.ClearInsertionPoint();
    // WhileBodyBB[8], i < j?
    builder.SetInsertPoint(quickSortWhileBodyBBs[8]);
    i = builder.CreateLoad(iAlloca);
    j = builder.CreateLoad(jAlloca);
    i_lt_j = builder.CreateICmpSLT(i, j);
    builder.CreateCondBr(i_lt_j, quickSortWhileBodyBBs[9],
                         quickSortWhileCondBB);
    builder.ClearInsertionPoint();
    // WhileBodyBB[9], arr[j--] = arr[i];
    builder.SetInsertPoint(quickSortWhileBodyBBs[9]);
    i = builder.CreateLoad(iAlloca);
    j = builder.CreateLoad(jAlloca);
    arr_i_pointer = builder.CreateGEP(arr, i);
    arr_j_pointer = builder.CreateGEP(arr, j);
    arr_i = builder.CreateLoad(arr_i_pointer);
    builder.CreateStore(arr_i, arr_j_pointer);
    j_minus_1 = builder.CreateSub(j, CONST(1));
    builder.CreateStore(j_minus_1, jAlloca);
    builder.CreateBr(quickSortWhileCondBB);
    builder.ClearInsertionPoint();
    // WhileExitBB, arr[i] = k; 并递归调用
    builder.SetInsertPoint(quickSortWhileExitBB);
    i = builder.CreateLoad(iAlloca);
    arr_i_pointer = builder.CreateGEP(arr, i);
    builder.CreateStore(k, arr_i_pointer);
    auto *i_minus_1 = builder.CreateSub(i, CONST(1));
    i_plus_1 = builder.CreateAdd(i, CONST(1));
    builder.CreateCall(quickSortFun, {arr, low, i_minus_1});
    builder.CreateCall(quickSortFun, {arr, i_plus_1, high});
    builder.CreateRetVoid();
    builder.ClearInsertionPoint();
    // 退出
    builder.SetInsertPoint(quickSortExitBB);
    builder.CreateRetVoid();
    builder.ClearInsertionPoint();

    // main 函数
    auto *mainFun = Function::Create(llvm::FunctionType::get(IntType, false),
                                     GlobalValue::LinkageTypes::ExternalLinkage,
                                     "main", module);
    auto *mainBB = BasicBlock::Create(context, "entry", mainFun);
    builder.SetInsertPoint(mainBB);
    auto *array_type = llvm::ArrayType::get(FloatType, 4096);
    auto *array = builder.CreateAlloca(array_type);
    // 初始化
    auto *array_pointer_cast = builder.CreateBitCast(array, Int8PtrType);
    builder.CreateMemSet(array_pointer_cast, CONST(0), 16384, MaybeAlign());
    // 读入数据
    auto *array_pointer = builder.CreateGEP(array, {CONST(0), CONST(0)});
    auto *array_num =
        builder.CreateCall(runtime->get_float_array_func, {array_pointer});
    // start 时间
    auto *start_time = builder.CreateCall(runtime->current_time_func);
    // 调用 quick_sort 函数
    auto *hi_argu = builder.CreateSub(array_num, CONST(1));
    builder.CreateCall(quickSortFun, {array_pointer, CONST(0), hi_argu});
    // end 时间
    auto *end_time = builder.CreateCall(runtime->current_time_func);
    // 输出数组
    builder.CreateCall(runtime->put_float_array_func,
                       {array_num, array_pointer});
    // 计算时间
    auto *time_cost = builder.CreateSub(end_time, start_time);
    // 输出时间
    builder.CreateCall(runtime->put_int_func, {time_cost});
    // 逐个输出空白，m，s，换行
    builder.CreateCall(runtime->put_char_func, {CONST(32)});
    builder.CreateCall(runtime->put_char_func, {CONST(109)});
    builder.CreateCall(runtime->put_char_func, {CONST(115)});
    builder.CreateCall(runtime->put_char_func, {CONST(10)});
    // 返回，退出
    builder.CreateRet(CONST(0));
    builder.ClearInsertionPoint();

    return module;
}
