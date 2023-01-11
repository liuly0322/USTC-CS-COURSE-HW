#include "BasicBlock.h"
#include "Constant.h"
#include "Function.h"
#include "IRStmtBuilder.h"
#include "Module.h"
#include "Type.h"

#include <iostream>
#include <memory>

#ifdef DEBUG  // 用于调试信息,大家可以在编译过程中通过" -DDEBUG"来开启这一选项
#define DEBUG_OUTPUT std::cout << __LINE__ << std::endl;  // 输出行号的简单示例
#else
#define DEBUG_OUTPUT
#endif

#define CONST_INT(num) \
    ConstantInt::get(num, module)

#define CONST_FP(num) \
    ConstantFloat::get(num, module) // 得到常数值的表示,方便后面多次用到

int main() {
  auto module = new Module("SysY code");  // module name是什么无关紧要
  auto builder = new IRStmtBuilder(nullptr, module);
  Type *Int32Type = Type::get_int32_type(module);

  // 全局数组,num,x
  auto *arrayType_num = ArrayType::get(Int32Type, 2);
  auto *arrayType_x = ArrayType::get(Int32Type, 1);
  auto zero_initializer = ConstantZero::get(Int32Type, module);
  std::vector<Constant *> init_val;
  init_val.push_back(CONST_INT(4));
  init_val.push_back(CONST_INT(8));
  auto num_initializer = ConstantArray::get(arrayType_num, init_val);
  auto num = GlobalVariable::create("num", module, arrayType_num, false, num_initializer);//          是否是常量定义，初始化常量(ConstantZero类)
  auto x = GlobalVariable::create("x", module, arrayType_x, false, zero_initializer);// 参数解释：  名字name，所属module，全局变量类型type，

  auto n = GlobalVariable::create("n", module, Int32Type, false, zero_initializer);
  auto tmp = GlobalVariable::create("tmp", module, Int32Type, false, CONST_INT(1));

  // climbStairs函数
  // 函数参数类型的vector
  std::vector<Type *> Ints(1, Int32Type);

  //通过返回值类型与参数类型列表得到函数类型
  auto climbStairsFunTy = FunctionType::get(Int32Type, Ints);

  // 由函数类型得到函数
  auto climbStairsFun = Function::create(climbStairsFunTy,
                                 "climbStairs", module);

  // BB的名字在生成中无所谓,但是可以方便阅读
  auto bb = BasicBlock::create(module, "entry", climbStairsFun);
  
  builder->set_insert_point(bb);                        // 一个BB的开始,将当前插入指令点的位置设在bb
  
  auto retAlloca = builder->create_alloca(Int32Type);   // 在内存中分配返回值的位置
  auto nAlloca = builder->create_alloca(Int32Type);     // 在内存中分配参数n的位置

  std::vector<Value *> args;  // 获取climbStairs函数的形参,通过Function中的iterator
  for (auto arg = climbStairsFun->arg_begin(); arg != climbStairsFun->arg_end(); arg++) {
    args.push_back(*arg);   // * 号运算符是从迭代器中取出迭代器当前指向的元素
  }

  builder->create_store(args[0], nAlloca);  // store参数n

  auto retBB = BasicBlock::create(
      module, "", climbStairsFun);  // return分支,提前create,以便true分支可以br

  auto nLoad = builder->create_load(nAlloca);           // 将参数n load上来
  auto icmp = builder->create_icmp_lt(nLoad, CONST_INT(4));  // n和4的比较,注意ICMPLT

  auto trueBB = BasicBlock::create(module, "trueBB_if", climbStairsFun);    // true分支
  auto falseBB = BasicBlock::create(module, "falseBB_if", climbStairsFun);  // false分支

  builder->create_cond_br(icmp, trueBB, falseBB);  // 条件BR
  DEBUG_OUTPUT // 我调试的时候故意留下来的,以醒目地提醒你这个调试用的宏定义方法
  builder->set_insert_point(trueBB);  // if true; 分支的开始需要SetInsertPoint设置
  nLoad = builder->create_load(nAlloca);
  builder->create_store(nLoad, retAlloca);
  builder->create_br(retBB);  // br retBB

  builder->set_insert_point(falseBB);  // if false
  auto *arrayType_dp = ArrayType::get(Int32Type, 10);
  auto dpAlloca = builder->create_alloca(arrayType_dp);

  auto dp0Gep = builder->create_gep(dpAlloca, {CONST_INT(0), CONST_INT(0)});
  builder->create_store(CONST_INT(0), dp0Gep);

  auto dp1Gep = builder->create_gep(dpAlloca, {CONST_INT(0), CONST_INT(1)});
  builder->create_store(CONST_INT(1), dp1Gep);

  auto dp2Gep = builder->create_gep(dpAlloca, {CONST_INT(0), CONST_INT(2)});
  builder->create_store(CONST_INT(2), dp2Gep);

  auto iAlloca = builder->create_alloca(Int32Type);
  builder->create_store(CONST_INT(3), iAlloca);

  auto condBB = BasicBlock::create(module, "condBB_while", climbStairsFun);  // 条件BB
  trueBB = BasicBlock::create(module, "trueBB_while", climbStairsFun);    // true分支
  falseBB = BasicBlock::create(module, "falseBB_while", climbStairsFun);  // false分支

  builder->create_br(condBB);

  builder->set_insert_point(condBB);

  auto iLoad = builder->create_load(iAlloca);
  nLoad = builder->create_load(nAlloca);
  auto add = builder->create_iadd(nLoad, CONST_INT(1));

  icmp = builder->create_icmp_lt(iLoad, add);
  builder->create_cond_br(icmp, trueBB, falseBB);

  builder->set_insert_point(trueBB);

  iLoad = builder->create_load(iAlloca);
  auto sub = builder->create_isub(iLoad, CONST_INT(1));
  auto dpGep = builder->create_gep(dpAlloca, {CONST_INT(0), sub});
  auto dp1Load = builder->create_load(dpGep);

  iLoad = builder->create_load(iAlloca);
  sub = builder->create_isub(iLoad, CONST_INT(2));
  dpGep = builder->create_gep(dpAlloca, {CONST_INT(0), sub});
  auto dp2Load = builder->create_load(dpGep);

  add = builder->create_iadd(dp1Load, dp2Load);
  iLoad = builder->create_load(iAlloca);
  dpGep = builder->create_gep(dpAlloca, {CONST_INT(0), iLoad});
  builder->create_store(add, dpGep);

  iLoad = builder->create_load(iAlloca);
  add = builder->create_iadd(iLoad, CONST_INT(1));
  builder->create_store(add, iAlloca);

  builder->create_br(condBB);

  builder->set_insert_point(falseBB);
  nLoad = builder->create_load(nAlloca);
  dpGep = builder->create_gep(dpAlloca, {CONST_INT(0), nLoad});
  auto dpLoad = builder->create_load(dpGep);
  builder->create_store(dpLoad, retAlloca);
  builder->create_br(retBB);

  builder->set_insert_point(retBB);  // ret分支
  auto retLoad = builder->create_load(retAlloca);
  builder->create_ret(retLoad);

  // main函数
  auto mainFun = Function::create(FunctionType::get(Int32Type, {}),
                                  "main", module);
  bb = BasicBlock::create(module, "entry", mainFun);
  // BasicBlock的名字在生成中无所谓,但是可以方便阅读
  builder->set_insert_point(bb);

  retAlloca = builder->create_alloca(Int32Type);
  auto resAlloca = builder->create_alloca(Int32Type);

  auto num0Gep = builder->create_gep(num, {CONST_INT(0), CONST_INT(0)});  // GEP: 这里为什么是{0, 0}呢? (实验报告相关)
  auto num0Load = builder->create_load(num0Gep);
  builder->create_store(num0Load, n);

  auto tmpLoad = builder->create_load(tmp);
  auto numGep = builder->create_gep(num, {CONST_INT(0), tmpLoad});
  auto numLoad = builder->create_load(numGep);
  auto x0Gep = builder->create_gep(x, {CONST_INT(0), CONST_INT(0)});
  builder->create_store(numLoad, x0Gep);

  nLoad = builder->create_load(n);
  tmpLoad = builder->create_load(tmp);
  add = builder->create_iadd(nLoad, tmpLoad);
  auto call = builder->create_call(climbStairsFun, {add});           // 为什么这里传的是{add}呢?
  builder->create_store(call, resAlloca);

  auto resLoad = builder->create_load(resAlloca);
  x0Gep = builder->create_gep(x, {CONST_INT(0), CONST_INT(0)});
  auto x0Load = builder->create_load(x0Gep);
  sub = builder->create_isub(resLoad, x0Load);
  builder->create_store(sub, retAlloca);
  retLoad = builder->create_load(retAlloca);
  builder->create_ret(retLoad);
  // 给这么多注释了,但是可能你们还是会弄很多bug
  // 所以强烈建议配置AutoComplete,效率会大大提高!
  // 别人配了AutoComplete,只花1小时coding
  // 你没有配AutoComplete,找method花5小时,debug花5小时,肯定哭唧唧!
  // 最后,如果猜不到某个IR指令对应的C++的函数,建议把指令翻译成英语然后在method列表中搜索一下
  // 最后的最后,这个例子只涉及到了一点基本的指令生成,
  // 对于额外的指令,包括数组,在之后的实验中可能需要大家好好搜索一下思考一下,
  // 还有涉及到的C++语法,可以在gitlab上发issue提问或者向大家提供指导
  // 对于这个例子里的代码风格/用法,如果有好的建议也欢迎提出!
  std::cout << module->print();
  delete module;
  return 0;
}
