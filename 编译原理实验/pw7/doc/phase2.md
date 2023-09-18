- [任务描述](#任务描述)
- [相关知识](#相关知识)
  - [SysYF IR 应用编程接口](#sysyf-ir-应用编程接口)
  - [样例学习](#样例学习)
- [本关具体任务](#本关具体任务)
- [编译、运行和验证](#编译运行和验证)
  - [运行说明](#运行说明)
- [思考题](#思考题)

---

### 任务描述
**本关任务**：首先，熟悉 SysYF IR 的应用编程接口。然后，通过调用 SysYF IR 应用编程接口，为给定的 5 个 SysYF 程序手工编写 C++ 代码，以生成并输出与 sy 文件功能相同的 ll 文件。

### 相关知识
#### SysYF IR 应用编程接口
由于 LLVM IR 官方的 C++ 应用编程接口的文档内容繁多，本实训项目提供 SysYF IR 应用编程接口库，该库用 C++ 编写，可以用于生成 LLVM IR 的子集。你需要阅读**SysYF IR 核心类的介绍**`SysYFIR.md`。  
本关要求你根据 AST，使用 SysYF IR 应用编程接口来为特定 SysYF 程序手工构建 LLVM IR。你需要先仔细阅读文档`doc/SysYFIR.md`以了解其接口的设计。

#### 样例学习
<details>
  <summary> go_upstairs_gen.cpp 核心部分（点击展开） </summary>

```cpp
    // 全局数组，num,x
    auto *arrayType_num = ArrayType::get(Int32Type, 2);
    auto *arrayType_x = ArrayType::get(Int32Type, 1);
    auto zero_initializer = ConstantZero::get(Int32Type, module);
    std::vector<Constant *> init_val;
    init_val.push_back(CONST_INT(4));
    init_val.push_back(CONST_INT(8));
    auto num_initializer = ConstantArray::get(arrayType_num, init_val);
    auto num = GlobalVariable::create("num", module, arrayType_num, false, num_initializer);//          是否是常量定义，初始化常量 (ConstantZero 类)
    auto x = GlobalVariable::create("x", module, arrayType_x, false, zero_initializer);// 参数解释：名字 name，所属 module，全局变量类型 type，

    auto n = GlobalVariable::create("n", module, Int32Type, false, zero_initializer);
    auto tmp = GlobalVariable::create("tmp", module, Int32Type, false, CONST_INT(1));

    // climbStairs 函数
    // 函数参数类型的 vector
    std::vector<Type *> Ints(1, Int32Type);

    //通过返回值类型与参数类型列表得到函数类型
    auto climbStairsFunTy = FunctionType::get(Int32Type, Ints);

    // 由函数类型得到函数
    auto climbStairsFun = Function::create(climbStairsFunTy,
                                    "climbStairs", module);

    // BB 的名字在生成中无所谓，但是可以方便阅读
    auto bb = BasicBlock::create(module, "entry", climbStairsFun);

    builder->set_insert_point(bb);                        // 一个 BB 的开始，将当前插入指令点的位置设在 bb

    auto retAlloca = builder->create_alloca(Int32Type);   // 在内存中分配返回值的位置
    auto nAlloca = builder->create_alloca(Int32Type);     // 在内存中分配参数 n 的位置

    std::vector<Value *> args;  // 获取 climbStairs 函数的形参，通过 Function 中的 iterator
    for (auto arg = climbStairsFun->arg_begin(); arg != climbStairsFun->arg_end(); arg++) {
    args.push_back(*arg);   // * 号运算符是从迭代器中取出迭代器当前指向的元素
    }

    builder->create_store(args[0], nAlloca);  // store 参数 n

    auto retBB = BasicBlock::create(
        module, "", climbStairsFun);  // return 分支，提前 create，以便 true 分支可以 br

    auto nLoad = builder->create_load(nAlloca);           // 将参数 n load 上来
    auto icmp = builder->create_icmp_lt(nLoad, CONST_INT(4));  // n 和 4 的比较，注意 ICMPLT

    auto trueBB = BasicBlock::create(module, "trueBB_if", climbStairsFun);    // true 分支
    auto falseBB = BasicBlock::create(module, "falseBB_if", climbStairsFun);  // false 分支

    builder->create_cond_br(icmp, trueBB, falseBB);  // 条件 BR
    DEBUG_OUTPUT // 我调试的时候故意留下来的，以醒目地提醒你这个调试用的宏定义方法
    builder->set_insert_point(trueBB);  // if true; 分支的开始需要 SetInsertPoint 设置
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

    auto condBB = BasicBlock::create(module, "condBB_while", climbStairsFun);  // 条件 BB
    trueBB = BasicBlock::create(module, "trueBB_while", climbStairsFun);    // true 分支
    falseBB = BasicBlock::create(module, "falseBB_while", climbStairsFun);  // false 分支

    builder->create_br(condBB);

    builder->set_insert_point(condBB);
    //后略，详细见代码文件
```
</details>

为了更直观地感受并学会使用 SysYF IR 应用编程接口，本实训项目提供了示例代码，位于`Student/task2/demo/go_upstairs_gen.cpp`。  
该 C++ 程序会生成与 go_upstairs.c 逻辑相同的 LLVM IR 文件，在该 C++ 程序中提供了详尽的注释，请阅读理解，以便更好地开展你的实验！  

### 本关具体任务
1. 你需要在`Student/task2/cpp/`文件夹中，调用 SysYF IR 应用编程接口，编写自己的 `assign_gen.cpp`、`func_gen.cpp`、`if_gen.cpp`、`while_gen.cpp`和`io_gen.cpp`程序，以生成与第 1 关的五个 sy 程序相同逻辑功能的 ll 文件。
2. 用 SysYF 语言编写一个快排程序`qsort.sy`，它读取元素个数 n 和待排序的整型数组，然后使用快排排序并最终输出排序好的数组。该程序至少包含读入待排序整型数组的函数、划分函数、交换函数、快排函数。编写相应的 cpp 文件`qsort_gen.cpp`，以生成相同逻辑功能的 ll 文件。你需要提供`qsort.in`输入数据和`qsort.out`输出数据。
3. 在`report.md`内回答[思考题](#思考题)

### 编译、运行和验证
- 在 `Student/task2/build/` 下执行：
``` shell
# 如果存在 CMakeCache.txt，要先删除
# rm CMakeCache.txt
cmake ..
make
```
你可以得到与 `assign_gen.cpp`、`func_gen.cpp`、`if_gen.cpp`、`while_gen.cpp`、`go_upstairs_gen.cpp`、`io_gen.cpp`、`qsort_gen.cpp`分别对应的可执行文件`assign_generator`、`func_generator`、`if_generator`、`while_generator`、`go_upstairs_generator`、`io_generator`、`qsort_generator`。  
- 之后直接执行可执行文件，即可得到对应的 ll 文件：  
``` shell
# 在build文件夹内
./go_upstairs_generator
```

#### 运行说明
- 你需要使用 `clang --version` 和 `lli --version` 检查本机的 Clang 和 LLVM 版本，要求版本为 11.0.0
- 你可以使用 `which lli` 来查找 `lli` 命令的位置
- 利用 LLVM 的命令 `lli`，可以执行`*.ll`文件；如果版本过低，可能会遇到`error: expected top-level entity`等问题
- 你也可以使用 `clang go_upstairs.ll -o go_upstairs` 来生成可执行文件
- `$?`的内容是上一条命令所返回的结果，而`echo $?`可以将其输出到终端中
- 使用`clang`时，注意扩展名为`sy`的文件是 SysYF 语言的程序文件，`clang`是无法直接识别的。你可以将`sy`文件复制为`c`文件来用`clang`编译
- 对于包含运行库函数的`io.sy`，在使用`clang`生成二进制文件时需要将`lib/lib.c`也包含在编译命令中。包含运行时库函数调用的`ll`文件无法用`lli`运行

### 思考题
请在`report/report.md`中详细回答下述思考题：

2-1. 请给出`SysYFIR.md`中提到的两种 getelementptr 用法的区别，并解释原因：
  - `%2 = getelementptr [10 x i32], [10 x i32]* %1, i32 0, i32 %0` 
  - `%2 = getelementptr i32, i32* %1, i32 %0`