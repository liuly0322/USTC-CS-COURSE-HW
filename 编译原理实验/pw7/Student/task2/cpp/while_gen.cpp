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

    // 基本块
    auto *whileCond1BB = BasicBlock::create(module, "while_cond1", mainFun);
    auto *whileCond2BB = BasicBlock::create(module, "while_cond2", mainFun);
    auto *whileInnerBB = BasicBlock::create(module, "while_inner", mainFun);
    auto *whileOuterBB = BasicBlock::create(module, "while_outer", mainFun);

    // 分配 b，初始化 a 和 b
    builder->create_store(CONST_INT(3), aAlloca);
    auto *bAlloca = builder->create_alloca(Int32Type);
    builder->create_store(CONST_INT(0), bAlloca);
    builder->create_br(whileCond1BB);

    // a > 0?
    builder->set_insert_point(whileCond1BB);
    auto *a = builder->create_load(aAlloca);
    auto *a_gt_0 = builder->create_icmp_gt(a, CONST_INT(0));
    builder->create_cond_br(a_gt_0, whileCond2BB, whileOuterBB);

    // b < 10?
    builder->set_insert_point(whileCond2BB);
    auto *b = builder->create_load(bAlloca);
    auto *b_lt_10 = builder->create_icmp_lt(b, CONST_INT(10));
    builder->create_cond_br(b_lt_10, whileInnerBB, whileOuterBB);

    // while inner
    builder->set_insert_point(whileInnerBB);
    auto *b_new = builder->create_iadd(b, a);
    auto *a_new = builder->create_isub(a, CONST_INT(1));
    builder->create_store(b_new, bAlloca);
    builder->create_store(a_new, aAlloca);
    builder->create_br(whileCond1BB);

    // while outer
    builder->set_insert_point(whileOuterBB);
    builder->create_ret(CONST_INT(0));

    std::cout << module->print();
    delete module;
    return 0;
}
