#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>

#include <iostream>
#include <memory>
#include "runtime/runtime.h"

using namespace llvm;  // 指明命名空间为llvm

#define CONST(num) \
    ConstantInt::get(context, APInt(32, num))  //得到常数值的表示,方便后面多次用到

llvm::Module* gcd_generator(LLVMContext &context, runtime_info* &runtime){
    IRBuilder<> builder(context);
    llvm::Type *TYPE32 = llvm::Type::getInt32Ty(context);
    // IRBuilder<> builder(context);
    auto module = new llvm::Module("gcd", context);  // module name是什么无关紧要

    runtime = new runtime_info(module);
    // 函数参数类型的vector
    std::vector<llvm::Type *> Ints(2, TYPE32);
    auto gcdFun = Function::Create(llvm::FunctionType::get(TYPE32, Ints, false),
                                    GlobalValue::LinkageTypes::ExternalLinkage,
                                    "gcd", module);

    // BB的名字在生成中无所谓,但是可以方便阅读
    auto bb = BasicBlock::Create(context, "entry", gcdFun);
    builder.SetInsertPoint(bb);                     // 一个BB的开始
    auto retAlloca = builder.CreateAlloca(TYPE32);  // 返回值的空间分配
    auto uAlloca = builder.CreateAlloca(TYPE32);    // 参数u的空间分配
    auto vAlloca = builder.CreateAlloca(TYPE32);    // 参数v的空间分配

    std::vector<Value *> args;  //获取gcd函数的参数,通过iterator
    for (auto arg = gcdFun->arg_begin(); arg != gcdFun->arg_end(); arg++) {
        args.push_back(arg);
    }

    builder.CreateStore(args[0], uAlloca);  //将参数u store下来
    builder.CreateStore(args[1], vAlloca);  //将参数v store下来

    auto vLoad = builder.CreateLoad(vAlloca);           //将参数v load上来
    auto icmp = builder.CreateICmpEQ(vLoad, CONST(0));  // v和0的比较,注意ICMPEQ

    auto trueBB = BasicBlock::Create(context, "trueBB", gcdFun);    // true分支
    auto falseBB = BasicBlock::Create(context, "falseBB", gcdFun);  // false分支
    auto retBB = BasicBlock::Create(
        context, "", gcdFun);  // return分支,提前create,以便true分支可以br

    auto br = builder.CreateCondBr(icmp, trueBB, falseBB);  // 条件BR
    builder.SetInsertPoint(trueBB);  // if true; 分支的开始需要SetInsertPoint设置
    auto uLoad = builder.CreateLoad(uAlloca);
    builder.CreateStore(uLoad, retAlloca);
    builder.CreateBr(retBB);  // br retBB

    builder.SetInsertPoint(falseBB);  // if false
    uLoad = builder.CreateLoad(uAlloca);
    vLoad = builder.CreateLoad(vAlloca);
    auto div = builder.CreateSDiv(uLoad, vLoad);  // SDIV - div with S flag
    auto mul = builder.CreateNSWMul(div, vLoad);  // NSWMUL - mul with NSW flags
    auto sub = builder.CreateNSWSub(uLoad, mul);  // the same
    auto call = builder.CreateCall(gcdFun, {vLoad, sub});
    // {vLoad, sub} - 参数array
    builder.CreateStore(call, retAlloca);
    builder.CreateBr(retBB);  // br retBB

    builder.SetInsertPoint(retBB);  // ret分支
    auto retLoad = builder.CreateLoad(retAlloca);
    builder.CreateRet(retLoad);

    // main函数
    auto mainFun = Function::Create(llvm::FunctionType::get(TYPE32, false),
                                    GlobalValue::LinkageTypes::ExternalLinkage,
                                    "main", module);
    bb = BasicBlock::Create(context, "entry", mainFun);
    // BasicBlock的名字在生成中无所谓,但是可以方便阅读
    builder.SetInsertPoint(bb);

    auto xAlloca = builder.CreateAlloca(TYPE32);
    auto yAlloca = builder.CreateAlloca(TYPE32);
    auto tempAlloca = builder.CreateAlloca(TYPE32);

    builder.CreateStore(CONST(0), tempAlloca);
    builder.CreateStore(CONST(72), xAlloca);
    builder.CreateStore(CONST(18), yAlloca);

    auto xLoad = builder.CreateLoad(xAlloca);
    auto yLoad = builder.CreateLoad(yAlloca);
    icmp = builder.CreateICmpSLT(xLoad, yLoad);  // ICMPLT with S

    trueBB = BasicBlock::Create(context, "trueBB", mainFun);
    falseBB = BasicBlock::Create(context, "falseBB", mainFun);
    builder.CreateCondBr(icmp, trueBB, falseBB);

    builder.SetInsertPoint(trueBB);
    builder.CreateStore(xLoad, tempAlloca);
    builder.CreateStore(yLoad, xAlloca);
    auto tempLoad = builder.CreateLoad(tempAlloca);
    builder.CreateStore(tempLoad, yAlloca);
    builder.CreateBr(falseBB);  // 注意在下一个BB之前要Br一下

    builder.SetInsertPoint(falseBB);
    xLoad = builder.CreateLoad(xAlloca);
    yLoad = builder.CreateLoad(yAlloca);
    call = builder.CreateCall(gcdFun, {xLoad, yLoad});

    auto print_call = builder.CreateCall(runtime->put_int_func, {call});
    print_call = builder.CreateCall(runtime->put_char_func, {CONST('\n')});

    builder.CreateRet(call);
    builder.ClearInsertionPoint();
    return module;
}
