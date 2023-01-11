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

    // add 函数
    const std::vector<Type *> params(2, Int32Type);
    auto *addFunTy = FunctionType::get(Int32Type, params);
    auto *addFun = Function::create(addFunTy, "add", module);
    auto *addBB = BasicBlock::create(module, "entry", addFun);
    builder->set_insert_point(addBB);
    auto *aParam = addFun->get_args().front();
    auto *bParam = addFun->get_args().back();
    auto *sum = builder->create_iadd(aParam, bParam);
    builder->create_ret(sum);

    // main 函数及入口
    auto *mainFun =
        Function::create(FunctionType::get(Int32Type, {}), "main", module);
    auto *mainBB = BasicBlock::create(module, "entry", mainFun);
    builder->set_insert_point(mainBB);

    // 分配 a 和 c，并初始化
    auto *aAlloca = builder->create_alloca(Int32Type);
    builder->create_store(CONST_INT(3), aAlloca);
    auto *cAlloca = builder->create_alloca(Int32Type);
    builder->create_store(CONST_INT(5), cAlloca);

    // 加载 a，作为 add 函数的第一个参数
    auto *a = builder->create_load(Int32Type, aAlloca);
    // 第二个参数是 a + a
    auto *a_plus_a = builder->create_iadd(a, a);
    // 函数调用
    auto *add_res = builder->create_call(addFun, {a, a_plus_a});

    // 计算返回值
    auto *c = builder->create_load(cAlloca);
    auto *ret_val = builder->create_iadd(c, add_res);
    builder->create_ret(ret_val);

    std::cout << module->print();
    delete module;
    return 0;
}
