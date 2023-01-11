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
    Type *Int32Type = Type::get_int32_type(module_ptr);
    Type *FloatType = Type::get_float_type(module_ptr);
    Type *Floatby10000Type = Type::get_array_type(FloatType, 10000);

    auto *getint = scope->find("getint", true);
    auto *putint = scope->find("putint", true);
    auto *getfarray = scope->find("getfarray", true);
    auto *putfarray = scope->find("putfarray", true);

    auto *mainFun =
        Function::create(FunctionType::get(Int32Type, {}), "main", module_ptr);
    auto *bb = BasicBlock::create(module_ptr, "entry", mainFun);
    stmt_builder->set_insert_point(bb);

    auto *bAlloca = stmt_builder->create_alloca(Floatby10000Type);
    auto *a = stmt_builder->create_call(getint, {});
    stmt_builder->create_call(putint, {a});
    auto *b0_p =
        stmt_builder->create_gep(bAlloca, {CONST_INT(0), CONST_INT(0)});
    auto *b1_p =
        stmt_builder->create_gep(bAlloca, {CONST_INT(0), CONST_INT(1)});
    stmt_builder->create_store(CONST_FP(1.0), b0_p);
    stmt_builder->create_store(CONST_FP(2.0), b1_p);
    auto *n = stmt_builder->create_call(getfarray, {b0_p});
    auto *n_plus_1 = stmt_builder->create_iadd(n, CONST_INT(1));
    stmt_builder->create_call(putfarray, {n_plus_1, b0_p});
    auto *b0 = stmt_builder->create_load(b0_p);
    auto *b0_int = stmt_builder->create_fptosi(b0, Int32Type);
    stmt_builder->create_ret(b0_int);

    std::cout << module_ptr->print();
    delete builder;
}