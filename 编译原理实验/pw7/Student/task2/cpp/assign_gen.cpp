#include "BasicBlock.h"
#include "Constant.h"
#include "Function.h"
#include "IRStmtBuilder.h"
#include "Module.h"
#include "Type.h"
#include <vector>

#define CONST_INT(num) ConstantInt::get(num, module)

#define CONST_FP(num) ConstantFloat::get(num, module)

int main() {
    // 初始化一些实例
    auto *module = new Module("assign_gen");
    auto *builder = new IRStmtBuilder(nullptr, module);
    Type *Int32Type = Type::get_int32_type(module);
    Type *FloatType = Type::get_float_type(module);
    Type *Int32by2_Type = Type::get_array_type(Int32Type, 2);

    // main 函数及入口
    auto *mainFun =
        Function::create(FunctionType::get(Int32Type, {}), "main", module);
    auto *bb = BasicBlock::create(module, "entry", mainFun);
    builder->set_insert_point(bb);

    // 分配 a 数组和 b，并初始化
    auto *aAlloca = builder->create_alloca(Int32by2_Type);
    auto *a0 = builder->create_gep(aAlloca, {CONST_INT(0), CONST_INT(0)});
    builder->create_store(CONST_INT(2), a0);
    auto *bAlloca = builder->create_alloca(FloatType);
    builder->create_store(CONST_FP(1.8), bAlloca);

    // 加载 a0 和 b，并运算
    auto *a0Load = builder->create_load(Int32Type, a0);
    auto *bLoad = builder->create_load(FloatType, bAlloca);
    auto *a0LoadFloat = builder->create_sitofp(a0Load, FloatType);
    auto *mulFloat = builder->create_fmul(a0LoadFloat, bLoad);
    auto *mulInt = builder->create_fptosi(mulFloat, Int32Type);
    auto *a1 = builder->create_gep(aAlloca, {CONST_INT(0), CONST_INT(1)});
    builder->create_store(mulInt, a1);
    auto *bLoadInt = builder->create_fptosi(bLoad, Int32Type);
    builder->create_ret(bLoadInt);

    std::cout << module->print();
    delete module;
    return 0;
}
