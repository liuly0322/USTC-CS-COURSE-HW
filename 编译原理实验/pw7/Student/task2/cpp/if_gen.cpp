#include "BasicBlock.h"
#include "Constant.h"
#include "Function.h"
#include "IRStmtBuilder.h"
#include "Module.h"
#include "Type.h"
#include <vector>

#define CONST_INT(num) ConstantInt::get(num, module)

int main() {
    // 初始化一些实例
    auto *module = new Module("assign_gen");
    auto *builder = new IRStmtBuilder(nullptr, module);
    Type *Int32Type = Type::get_int32_type(module);

    // 全局变量
    auto *zero_initializer = ConstantZero::get(Int32Type, module);
    auto *aAlloca =
        GlobalVariable::create("a", module, Int32Type, false, zero_initializer);

    // main 函数及入口
    auto *mainFun =
        Function::create(FunctionType::get(Int32Type, {}), "main", module);
    auto *entryBB = BasicBlock::create(module, "entry", mainFun);
    builder->set_insert_point(entryBB);

    // 初始化 a
    builder->create_store(CONST_INT(10), aAlloca);

    // 基本块
    auto *a_lt_6_BB = BasicBlock::create(module, "lt_6", mainFun);
    auto *successBB = BasicBlock::create(module, "success", mainFun);
    auto *failBB = BasicBlock::create(module, "fail", mainFun);

    // a > 0?
    auto *a = builder->create_load(aAlloca);
    auto *a_gt_0 = builder->create_icmp_gt(a, CONST_INT(0));
    builder->create_cond_br(a_gt_0, successBB, a_lt_6_BB);

    // a < 6?
    builder->set_insert_point(a_lt_6_BB);
    auto *a_lt_6 = builder->create_icmp_lt(a, CONST_INT(6));
    builder->create_cond_br(a_lt_6, successBB, failBB);

    // success, return a
    builder->set_insert_point(successBB);
    builder->create_ret(a);

    // fail, return -a
    builder->set_insert_point(failBB);
    auto *negative_a = builder->create_isub(CONST_INT(0), a);
    builder->create_ret(negative_a);

    std::cout << module->print();
    delete module;
    return 0;
}
