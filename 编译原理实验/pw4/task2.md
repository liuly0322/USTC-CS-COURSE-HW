# 使用Flex和Bison构建SysYF语言前端

- [使用Flex和Bison构建SysYF语言前端](#使用flex和bison构建sysyf语言前端)
  - [任务说明](#任务说明)
  - [目录结构](#目录结构)
  - [SysYF语言定义](#sysyf语言定义)
  - [评测说明](#评测说明)

**注意：请务必读懂demo后再进行本关实验**

## 任务说明

在本关，你将使用Bison,Flex工具，在我们给定的框架下实现SysYF(一个C语言子集)的前端。在本关，你**只需要**更改`grammar/sysyfParser.yy`和`grammar/sysyfScanner.ll`。在你需要填写代码的位置有相应注释说明。你需要使用本实验框架中**提供**的AST的结构来构建AST。对应AST的文档见`doc/AST.md`。对应的AST的声明和定义见`include/SyntaxTree.h`，`src/SyntaxTree.cpp`。评测时会调用`SyntaxTreePrinter`和正确生成的AST进行比较。`SyntaxTreePrinter`源代码见`include/SyntaxTreePrinter.h`，`src/SyntaxTreePrinter.cpp`。

**注意**，请不要修改除`grammar`外其他目录下的文件。

## 目录结构
```
.
├── CMakeLists.txt                  # 项目构建描述文档
├── SysYF语言定义.pdf
├── README.md
├── doc
│   └── AST.md                     # 抽象语法树接口描述文档
├── grammar
│   ├── sysyfParser.yy             # 语法分析器使用的语法描述文件，请在此文件中按照注释指示完成语法定义填写
│   └── sysyfScanner.ll            # 词法分析器使用的词法描述文件，请在此文件中按照注释指示完成词法定义填写
├── include
│   ├── ErrorReporter.h            # 报错相关的类声明
│   ├── SyntaxTree.h               # 抽象语法树节点类定义
│   ├── SyntaxTreePrinter.h        # 通过抽象语法树还原代码，相关的函数声明
│   ├── sysyfDriver.h              # 驱动词法语法分析器工作
│   └── sysyfFlexLexer.h           # 词法分析包装类
├── src
│   ├── ErrorReporter.cpp          # 报错相关的方法实现
│   ├── SyntaxTree.cpp             # 抽象语法树节点类的方法实现
│   ├── SyntaxTreePrinter.cpp      # 通过抽象语法树还原代码，相关的函数实现
│   ├── main.cpp                   # 主函数所在，调动整个模块工作，分析输入命令
│   └── sysyfDriver.cpp            # 驱动词法语法分析器工作   
└── test
    ├── <num>_<name>.out            # 测试样例预期输出文件
    └── <num>_<name>.sy             # 测试样例文件
```

## SysYF语言定义

SysYF语言定义见`SysYF语言定义.pdf`。该文档包含了部分语义说明(如同名标识符的约定等)，是后续实验需要的。本次实验不需要考虑语义。本关实验的评测输入一定是符合SysYF语言规范的合法程序，不存在未定义行为，且所有的测试集都是公开的。

## 评测说明

评测时会使用若干SysYF语言程序作为输入，评测输入一定是符合SysYF语言规范的合法程序，不存在未定义行为（如数组非对齐初始化，Int、Float溢出等。测试文件在test目录下，其中以`.sy`结尾的为测试样例，`.out`结尾的为应有的输出。
