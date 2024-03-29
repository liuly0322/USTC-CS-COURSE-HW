# 编译原理实验

编译原理的实验还是比较有趣的。

- pw2: 使用 antlr/flex/手工，分别构建一个简单正则文法的 lexer
- pw3: 一个关于智能指针的简单问答实验
- pw4: 使用 flex/bison 编写一个 SysYF 语言的前端（lexer + parser，转换成语法树），实验比较肝
- pw5: 在构建好的 parser 的基础上进行简单的静态分析
- pw6: 非必做，关于 C++ 类型转换的小实验/介绍
- pw7: 今年是熟悉语法树到中间表示（IR）的一套接口（但不需要编写语法树转换成 IR 的 IRBuilder），主要任务是编写 LLVM IR
  - 本人额外实现了 IRBuilder.cpp，可以见 `src/SysYFIRBuilder` 目录
  - 测试用例在 Student/task4 文件夹下
- pw8: 组队大实验，可选项如下：
  - 在 LLVM IR 上进行自选优化
  - 将 LLVM IR 生成为 ARM 汇编并尽可能优化生成逻辑

pw8 仓库：<https://github.com/liuly0322/sysyf_compiler_codegen_opt>。

总体来说，代码编译的流程可以划分为：

词法分析（生成 token 流），语法分析（构建语法树），语义分析（静态检查），中间代码生成，中间代码优化，中间代码构建到不同体系结构的后端。

一学期的实验基本涵盖了这些内容。
