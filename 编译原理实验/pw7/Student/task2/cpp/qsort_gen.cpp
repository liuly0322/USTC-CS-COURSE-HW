#include "BasicBlock.h"
#include "Constant.h"
#include "Function.h"
#include "IRBuilder.h"
#include "IRStmtBuilder.h"
#include "Module.h"
#include "Type.h"

#include <iostream>
#include <memory>

#define CONST_INT(num) ConstantInt::get(num, module_ptr)

#define CONST_FP(num) ConstantFloat::get(num, module_ptr)

int main() {
    auto *builder = new IRBuilder();
    auto module = builder->getModule();
    auto *module_ptr = module.get();
    auto *scope = builder->getScope();
    auto *stmt_builder = builder->getStmtBuilder();
    Type *VoidType = Type::get_void_type(module_ptr);
    Type *Int32Type = Type::get_int32_type(module_ptr);
    Type *Intby10000Type = Type::get_array_type(Int32Type, 10000);

    auto *getarray = scope->find("getarray", true);
    auto *putarray = scope->find("putarray", true);

    // 全局变量 a 数组
    auto *zero_initializer = ConstantZero::get(Intby10000Type, module_ptr);
    auto *aAlloca = GlobalVariable::create("a", module_ptr, Intby10000Type,
                                           false, zero_initializer);

    // read 函数
    auto *readFun =
        Function::create(FunctionType::get(Int32Type, {}), "read", module_ptr);
    auto *readEntryBB = BasicBlock::create(module_ptr, "entry", readFun);
    stmt_builder->set_insert_point(readEntryBB);
    auto *a_pointer =
        stmt_builder->create_gep(aAlloca, {CONST_INT(0), CONST_INT(0)});
    auto *ret_val = stmt_builder->create_call(getarray, {a_pointer});
    stmt_builder->create_ret(ret_val);

    // swap 函数
    auto *swapFun =
        Function::create(FunctionType::get(VoidType, {Int32Type, Int32Type}),
                         "swap", module_ptr);
    auto *swapEntryBB = BasicBlock::create(module_ptr, "entry", swapFun);
    stmt_builder->set_insert_point(swapEntryBB);
    auto *iParam = swapFun->get_args().front();
    auto *jParam = swapFun->get_args().back();
    auto *a_i_pointer =
        stmt_builder->create_gep(aAlloca, {CONST_INT(0), iParam});
    auto *a_j_pointer =
        stmt_builder->create_gep(aAlloca, {CONST_INT(0), jParam});
    auto *a_i_val = stmt_builder->create_load(a_i_pointer);
    auto *a_j_val = stmt_builder->create_load(a_j_pointer);
    stmt_builder->create_store(a_i_val, a_j_pointer);
    stmt_builder->create_store(a_j_val, a_i_pointer);
    stmt_builder->create_void_ret();

    // partition 函数
    auto *partitionFun =
        Function::create(FunctionType::get(Int32Type, {Int32Type, Int32Type}),
                         "partition", module_ptr);
    auto *partitionEntryBB =
        BasicBlock::create(module_ptr, "entry", partitionFun);
    auto *whileStartBB =
        BasicBlock::create(module_ptr, "while_start", partitionFun);
    auto *whileHiBB = BasicBlock::create(module_ptr, "while_hi", partitionFun);
    auto *ifCondBB = BasicBlock::create(module_ptr, "if_cond", partitionFun);
    auto *ifThenBB = BasicBlock::create(module_ptr, "if_then", partitionFun);
    auto *ifElseBB = BasicBlock::create(module_ptr, "if_else", partitionFun);

    stmt_builder->set_insert_point(partitionEntryBB);
    auto *loParam = swapFun->get_args().front();
    auto *hiParam = swapFun->get_args().back();
    auto *lo_plus_high = stmt_builder->create_iadd(loParam, hiParam);
    auto *mid = stmt_builder->create_isdiv(lo_plus_high, CONST_INT(2));
    auto *pivot_pointer =
        stmt_builder->create_gep(aAlloca, {CONST_INT(0), mid});
    auto *pivot = stmt_builder->create_load(pivot_pointer);
    auto *lo_minus_1 = stmt_builder->create_isub(loParam, CONST_INT(1));
    auto *hi_plus_1 = stmt_builder->create_iadd(hiParam, CONST_INT(1));
    auto *loAlloca = stmt_builder->create_alloca(Int32Type);
    auto *hiAlloca = stmt_builder->create_alloca(Int32Type);
    stmt_builder->create_store(lo_minus_1, loAlloca);
    stmt_builder->create_store(hi_plus_1, hiAlloca);
    stmt_builder->create_br(whileStartBB);
    // while 开始，更新 lo
    stmt_builder->set_insert_point(whileStartBB);
    auto *lo = stmt_builder->create_load(loAlloca);
    auto *lo_plus_1 = stmt_builder->create_iadd(lo, CONST_INT(1));
    stmt_builder->create_store(lo_plus_1, loAlloca);
    auto *a_lo_pointer =
        stmt_builder->create_gep(aAlloca, {CONST_INT(0), lo_plus_1});
    auto *a_lo = stmt_builder->create_load(a_lo_pointer);
    auto *a_lo_lt_pivot = stmt_builder->create_icmp_lt(a_lo, pivot);
    stmt_builder->create_cond_br(a_lo_lt_pivot, whileStartBB, whileHiBB);
    // 更新 hi
    stmt_builder->set_insert_point(whileHiBB);
    auto *hi = stmt_builder->create_load(hiAlloca);
    auto *hi_minus_1 = stmt_builder->create_isub(hi, CONST_INT(1));
    stmt_builder->create_store(hi_minus_1, hiAlloca);
    auto *a_hi_pointer =
        stmt_builder->create_gep(aAlloca, {CONST_INT(0), hi_minus_1});
    auto *a_hi = stmt_builder->create_load(a_hi_pointer);
    auto *a_hi_gt_pivot = stmt_builder->create_icmp_gt(a_hi, pivot);
    stmt_builder->create_cond_br(a_hi_gt_pivot, whileHiBB, ifCondBB);
    // if 判断
    stmt_builder->set_insert_point(ifCondBB);
    lo = stmt_builder->create_load(loAlloca);
    hi = stmt_builder->create_load(hiAlloca);
    auto *lo_ge_hi = stmt_builder->create_icmp_ge(lo, hi);
    stmt_builder->create_cond_br(lo_ge_hi, ifThenBB, ifElseBB);
    // return hi
    stmt_builder->set_insert_point(ifThenBB);
    stmt_builder->create_ret(hi);
    // swap and go back
    stmt_builder->set_insert_point(ifElseBB);
    stmt_builder->create_call(swapFun, {lo, hi});
    stmt_builder->create_br(whileStartBB);

    // quick_sort 函数
    auto *quickSortFun =
        Function::create(FunctionType::get(VoidType, {Int32Type, Int32Type}),
                         "quickSort", module_ptr);
    auto *quickSortEntryBB =
        BasicBlock::create(module_ptr, "entry", quickSortFun);
    auto *quickSortCondBB =
        BasicBlock::create(module_ptr, "cond", quickSortFun);
    auto *quickSortMainBB =
        BasicBlock::create(module_ptr, "main", quickSortFun);
    auto *quickSortExitBB =
        BasicBlock::create(module_ptr, "exit", quickSortFun);
    // 判断 lo >= hi 则退出
    stmt_builder->set_insert_point(quickSortEntryBB);
    loParam = swapFun->get_args().front();
    hiParam = swapFun->get_args().back();
    lo_ge_hi = stmt_builder->create_icmp_ge(loParam, hiParam);
    stmt_builder->create_cond_br(lo_ge_hi, quickSortExitBB, quickSortCondBB);
    // 判断 lo < 0 则退出
    stmt_builder->set_insert_point(quickSortCondBB);
    auto *lo_lt_0 = stmt_builder->create_icmp_lt(loParam, CONST_INT(0));
    stmt_builder->create_cond_br(lo_lt_0, quickSortExitBB, quickSortMainBB);
    // 主体部分
    stmt_builder->set_insert_point(quickSortMainBB);
    auto *pivot_ = stmt_builder->create_call(partitionFun, {loParam, hiParam});
    stmt_builder->create_call(quickSortFun, {loParam, pivot_});
    auto *pivot_plus_1 = stmt_builder->create_iadd(pivot_, CONST_INT(1));
    stmt_builder->create_call(quickSortFun, {pivot_plus_1, hiParam});
    stmt_builder->create_void_ret();
    // 退出部分
    stmt_builder->set_insert_point(quickSortExitBB);
    stmt_builder->create_void_ret();

    // main 函数
    auto *mainFun =
        Function::create(FunctionType::get(Int32Type, {}), "main", module_ptr);
    auto *mainEntryBB = BasicBlock::create(module_ptr, "entry", mainFun);
    stmt_builder->set_insert_point(mainEntryBB);
    auto *n = stmt_builder->create_call(readFun, {});
    auto *n_minus_1 = stmt_builder->create_isub(n, CONST_INT(1));
    stmt_builder->create_call(quickSortFun, {CONST_INT(0), n_minus_1});
    auto *a_0_pointer =
        stmt_builder->create_gep(aAlloca, {CONST_INT(0), CONST_INT(0)});
    stmt_builder->create_call(putarray, {n, a_0_pointer});
    stmt_builder->create_ret(CONST_INT(0));

    std::cout << module_ptr->print();
    delete builder;
}