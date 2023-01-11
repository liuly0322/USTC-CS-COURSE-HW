# PW6 实验报告

## 问题回答

### 1-1

请给出 while 语句对应的 LLVM IR 的代码布局特点，重点解释其中涉及的几个`br`指令的含义（包含各个参数的含义）。

一个可行的 while 语句对应 LLVM IR 的布局：

```llvm
while_start:
  ; 这里是条件判断，可能有多个基本块
  br i1 %cond, label %while_inner, label %while_out

while_inner:
  ; in while
  ; 这里做 while 内的操作
  ; 随后无条件跳转，再次判断是否满足循环条件
  br label %while_start

while_out:
  ; do other
```

条件判断部分可能由多个基本块组成，根据情况，`%cond` 可能需要使用 `phi` 聚合最后的判断结果。

如果条件判断为真，那么可以执行 while 内部的语句，否则结束循环。

在 while 内部的语句执行完后，无条件跳转至条件判断部分，再次判断循环条件是否满足。

### 1-2

请简述函数调用语句对应的 LLVM IR 的代码特点。

先计算实参，再调用 `call` 语句：

格式：`<result> = call <return ty> <func name>(<function args>) `

`call` 指令用于使控制流转移到指定的函数，其传入参数绑定到指定的值。参数类型需要具体指定。

在被调用函数中执行 `ret` 指令后，控制流程将在函数调用后继续执行该指令，并且该函数的返回值绑定到 `result` 参数。

### 2-1

请给出 `SysYFIR.md` 中提到的两种 getelementptr 用法的区别, 并解释原因:

- `%2 = getelementptr [10 x i32], [10 x i32]* %1, i32 0, i32 %0`
- `%2 = getelementptr i32, i32* %1, i32 %0`

这里的两种 getelementptr 没有本质区别，都是根据指针类型产生索引地址，区别在于二者的指针类型不同：

- 第一个语句是数组指针，因此实际上有两个维度，第一维度索引了数组，因此偏移量 40 字节，第二维度是数组内部的索引，偏移量 4 字节。
- 第二个语句是 i32 指针，只有一个维度。

下面的代码可以更好的帮助理解：

```c
#include <stdio.h>

int arr[10] = {1};
int (*arrPtr)[10] = &arr;
int *intPtr = NULL;

int main() {
  // 测试两种指针的偏移量
  int(*arrPtrType)[10] = arrPtr;
  printf("%d %d\n", arrPtrType, intPtr);
  arrPtrType++, intPtr++;
  printf("%d %d\n", arrPtrType, intPtr);

  // 测试数组指针的性质
  printf("%d %d %d\n", arrPtr, arrPtr[0], *arrPtr[0]);
}
```

输出：

```shell
4210736 0
4210776 4
4210736 4210736 1
```

值得注意的是，`arrPtr` 和 `arrPtr[0]` 的值是相等的，它们都作为指针，指向了数组的首地址，区别只是指针类型不同。

### 3-1

`src/driver.cpp` 中 `printASTUnit()` 方法通过编译器实例 `_Clang` 执行 `ASTDumpAction`，实现了解析抽象语法树并打印的功能。请根据 Clang driver 处理流程回答该方法包含了哪几个阶段？

事实上，Pipeline 阶段构建的不是 Action 的实例，而是根据输入文件的类型，构建 phase 的实例。根据 [Clang 文档](https://clang.llvm.org/doxygen/namespaceclang_1_1driver_1_1phases.html)，phase 有以下的成员：Preprocess,Precompile,Compile,Backend,Assemble,Link,IfsMerge

而我们的 `printASTUnit()` 实际的工作是往这些 phase 中增添 Action，也可以认为 Action 是在 Pipeline 和 Bind 之间的一个阶段。在执行 `printASTUnit()` 之后，相当于在 compile 的 phase 上指定了 `-ast-dump` 选项，这一选项会在后面的阶段中持续传递。

综上所述，可以认为 `printASTUnit()` 影响了所有 Bind 及之后的阶段。

### 3-2

根据 `include/driver.h` 中 `_Act` 类型，请简述 `src/driver.cpp` 中 `FrontendCodeGen()` 方法的功能。

`_Act` 类型是 `std::unique_ptr<CodeGenAction>`，提供了前端代码生成（到 LLVM IR）的 Action。

## 实验设计

（实验设计和难点应该是去年 task3 AST->IR lab 需要的吧。。。今年的实验本身感觉没啥可写的。）

### task1

要求写出 sy 代码对应的 LLVM IR。根据代码含义翻译即可。

### task2

调用接口翻译即可。

### task3

qsort 调用另一套接口翻译即可。

补全 runtime 函数参照示例即可。

## 实验难点及解决方案

今年的实验主要难点也就是理解 LLVM IR 以及相关接口。学习过后就很顺利了，体力活。

## 实验总结

- 本次实验使我基本了解了 LLVM IR 的语法和相关 C++ 接口。
- 本次实验使我加深了对编译过程以及代码优化时机（Frontend, Optimizer, Backend）的理解。
- 本次实验使我了解了 driver 的工作原理。
- 本次实验使我加深了对动态运行时库以及链接过程的理解。

## 实验反馈

- 本实验 _1.实验介绍_ 部分的文档在仓库里是看不到的，可以考虑把这部分文档拆分到具体的 phase（避免还需要回头再来看这个文档）。
- 实验过程中遇到了很多 segmentation fault，体验很不良好。框架有些地方可以或许增加判空，并打印具体错误位置。例如 phase2 线上测试如果找不到 .in 和 .out 文件会段错误，phase3 的 runtime 函数补全错误也会简单报段错误。
- phase3 的 runtime 一开始可以补全少量函数，最起码能把 gcd_generator 跑起来，不然有点让人不知所云。或者说可以考虑把第二个任务（补全 qsort.hpp）和第三个任务（补全 runtime）调换一下顺序。
- 手动写 LLVM IR 的部分有点重复了，可以考虑减少需要编写的 ll 文件数量。
- 私心建议 CMakeLists.txt 统一增加 `set(CMAKE_EXPORT_COMPILE_COMMANDS ON)`，clangd 实在太好用了。另外，可以考虑把代码用统一的 `clang-format` 和 `clang-tidy` 配置格式化一遍，并且把配置文件纳入版本控制。
  - 这个去年就有 [相关 issue #165](https://git.lug.ustc.edu.cn/compiler/course/-/issues/165) 了。
