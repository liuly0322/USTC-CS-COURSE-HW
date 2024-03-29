[TOC]

---

### 任务描述
**本关任务**：编写`IRBuilder.cpp`文件，实现低级中间代码生成器，为 SysYF 语言程序生成兼容的 LLVM IR 代码。

### 相关知识
#### 实验框架
本实训项目提供用 C++ 语言编写的 SysYF IR 应用编程库，用于构建 LLVM IR 的子集。为了简化你的实验，本实训的实验框架代码已完成了 SysYF 源程序到 C++ 上的抽象语法树的转换。

在`IRBuilder.h`中，还定义了一个用于存储作用域的类`Scope`。它的作用是在遍历语法树时，辅助管理不同作用域中的变量。它提供了以下接口：
```cpp
// 进入一个新的作用域
void enter();
// 退出一个作用域
void exit();
// 往当前作用域插入新的名字->值映射
bool push(std::string name, Value *val);
// 根据名字，以及是否为函数的 bool 值寻找到对应值
// isfunc 为 true 时为寻找函数，否则为寻找其他变量对应的值
Value* find(std::string name, bool isfunc);
// 判断当前是否在全局作用域内
bool in_global();
```
你需要根据语义合理调用`enter`与`exit`，并且在变量声明和使用时正确调用`push`与`find`。在类`SysYFIRBuilder`中，有一个`Scope`类型的成员变量`scope`，它在初始化时已经将特殊函数加入了作用域中。因此，你在进行名字查找时不需要顾虑是否需要对特殊函数进行特殊操作。

### 本关具体任务
1. 你需要在`src/SysYFIRBuilder`文件夹中，调用 SysYF IR 应用编程接口，填写`IRBuilder.cpp`文件，以实现 LLVM IR 的自动生成。
2. 在`report.md`内回答[思考题](#思考题)
3. 在`contribution.md`内由组长填写各组员的贡献比

### 编译、运行与验证

#### 编译运行 SysYFCompiler

```sh
mkdir build
cd build
cmake ..
make
```

编译后会产生 `SysYFCompiler` 程序，它能将 SysYF 源程序（sy 文件）输出为 LLVM IR。  
当需要对 sy 文件测试时，可以这样使用：

```sh
SysYFCompiler test.sy -emit-ir -o test.ll
```
得到对应的 ll 文件。

#### 自动测试

本实训项目提供了自动评测脚本，在`Student/task3/`目录下执行`python3 test.py`, 即可得到评测信息

### 思考题
请在`report/report.md`中详细回答下述思考题。

3-1. 在`scope`内单独处理`func`的好处有哪些。


### 选做

本实训项目提供了选做内容，若你能完成选做部分，将会有额外加分 (仅针对本次实验的团队代码得分，并且分数不能超过该部分得分上限)。

选做部分验收方式为线下验收，你需要在线下检查时提供对应代码通过助教给出的选做部分测试样例，并且讲解你的代码。

选做部分说明如下：
   - 多维数组


#### 多维数组

目前给出的 SysYF IR 应用编程接口并不支持多维数组的实现，因此你需要修改接口，以实现多维数组的声明、初始化和使用，你可以修改的内容为文件夹`include/SysYFIR`，`include/SysYFIRBuilder`，`src/SysYFIR`，`src/SysYFIRBuilder`内的所有内容

多维数组在目前的接口基础上，一般有两种做法：  
- 直接实现，可以参考 clang 生成的 LLVM IR，修改当前接口使其支持多维的数组类型
- 展平，把高维数组当成一维数组存储，修改当前接口使其能保存一些必要信息

在初始化多维数组时，与一维数组不同的是，存在对齐问题，这里假设多维数组的初始化为完全对齐（在每个大括号处均对齐），以下两种初始化是等价的：  
`int a[5][2] = {1,{2,3},{4},{5,6,7}}`  
`int a[5][2] = {{1,0},{2,3},{4,0},{5,6},{7,0}}`  

### 备注

测试样例除了位于版本库中的公开测例外，还包含不开放的隐藏测例。

平台上第三关的评测只判断公开测例是否完全通过，第三关的分数由助教线下检查后，根据公开测例和隐藏测例的通过情况确定，因此请自行设计合法测例测试你们的代码，确保你们的代码考虑了足够多情况以通过尽可能多的隐藏测例。

请将你编写的测例代码上传到`Student/task3/test_stu/`。
