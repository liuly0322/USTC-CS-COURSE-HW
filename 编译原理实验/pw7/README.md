# IR-Simple-Lab 实验文档
- [IR-Simple-Lab 实验文档](#ir-simple-lab-实验文档)
  - [0. 前言](#0-前言)
    - [安装LLVM 11.0.0](#安装llvm11.0.0)
    - [本次实验的主要内容](#本次实验的主要内容)
  - [1. 实验要求](#1-实验要求)
    - [1.1 目录结构](#11-目录结构)
    - [1.2 提交要求和评分标准](#12-提交要求和评分标准)

## 0. 前言

本次实验分为3关。**请务必使用git提交**。    
实验目的旨在让大家熟悉生成中间代码所需要的相关知识: LLVM IR、 SysYF IR（LLVM IR的轻量级C++接口）以及如何基于Clang/LLVM构建编译器的驱动程序Driver。
在开始实验之前，请确保LLVM的版本为11.0.0，且PATH环境变量配置正确。可以通过`lli --version`命令是否可以输出11.0.0的版本信息来验证。(虚拟平台上已经安装相应版本，可以通过前述命令进行检查)

### 安装LLVM 11.0.0
如果你在本机开展实验，你需要事先安装LLVM 11.0.0等软件。LLVM 11.0.0可以从[网址](https://releases.llvm.org/download.html#11.0.0)获取。

你可以选择`Pre-Built Binaries`中相应的压缩包解压得到二进制文件(linux)或者点击安装(windows)，然后添加到PATH(linux平台)或者添加到环境变量(windows，在安装包中有相应选项添加至系统路径)。

下面以Ubuntu20.04系统为例，说明安装步骤。

```
cd ~
wget https://github.com/llvm/llvm-project/releases/download/llvmorg-11.0.0/clang+llvm-11.0.0-x86_64-linux-gnu-ubuntu-20.04.tar.xz
tar -xvf clang+llvm-11.0.0-x86_64-linux-gnu-ubuntu-20.04.tar.xz
mv clang+llvm-11.0.0-x86_64-linux-gnu-ubuntu-20.04 llvm11.0.0
export PATH=~/llvm11.0.0/bin:$PATH
```

### 本次实验的主要内容

1. 第一部分: 了解LLVM IR。通过clang生成的.ll，了解LLVM IR与C程序代码的对应关系。相应文档见[phase1.md](./doc/phase1.md)
2. 第二部分: 了解SysYF IR。通过本实验提供的c++例子，了解SysYF IR的C++编程接口及实现。相应文档见[phase2.md](./doc/phase2.md)
3. 第三部分: 了解编译器驱动程序Driver。通过本实验提供的基于Clang Driver改造的可扩展LLVM驱动程序，了解LLVM的前后端处理流程，包括JIT即时编译执行。相应文档见[phase3.md](./doc/phase3.md)
4. 实验报告：在[report.md](./report/report.md)中撰写报告。

## 1. 实验要求

### 1.1 目录结构
除了下面特别指出的需要你修改或提交的文件，其他文件请勿随意修改。
``` log
.
├── CMakeLists.txt
├── README.md                           <- 实验文档说明（你在这里）
├── doc
│   ├── AST.md
│   ├── phase1.md                       <- 阶段1(第1关)的文档
│   ├── phase2.md                       <- 阶段2(第2关)的文档
│   ├── phase3.md                       <- 阶段3(第3关)的文档
│   ├── SysYF语言定义.pdf
|   ├── runtime.md                      <- SysYF的运行时库说明
|   └── SysYFIR.md                      <- SysYF IR 应用编程接口相关文档
├── report
│   ├── report.md                       <- 需提交的实验报告
├── lib                                 <- SysYF的运行时库
|   ├── lib.h                           <- 包含可以在SysYF程序中使用的I/O函数接口声明
|   └── lib.c
├── include                             <- 实验所需的头文件
│   ├── ...
│   └── SysYFIR
├── src
│   ├── ...
│   ├── SysYFIR
│   └── SysYFIRBuilder
|       ├── CMakeLists.txt
|       └── IRBuilder.cpp               <- SysYF IR的构建类
└── Student
    ├── task1						    <- 第1关任务相关的目录
    |   ├── ll                          <- 需提交的LLVM IR文件(.ll)的目录（第1关）
    |   |   ├── assign_hand.ll
    |   │   ├── fun_hand.ll
    |   │   ├── if_hand.ll
    |   |   ├── io_hand.ll
    |   │   └── while_hand.ll
    |   ├── sy
    |   |   ├── assign_test.sy
    |   │   ├── fun_test.sy
    |   │   ├── if_test.sy
    |   │   ├── io.sy
    |   │   ├── io.in
    |   │   ├── io.out
    |   │   └── while_test.sy
    |   └── demo
    |       └── go_upstairs.c
    ├── task2						    <- 第2关任务相关的目录
    ├── CMakeLists.txt
    |   ├── cpp                         <- 需提交的.cpp目录（第2关）
    |   |   ├── CMakeLists.txt
    |   |   ├── assign_gen.cpp
    |   │   ├── fun_gen.cpp
    |   │   ├── if_gen.cpp
    |   │   ├── io_gen.cpp
    |   │   ├── qsort_gen.cpp
    |   │   └── while_gen.cpp
    |   ├── sy
    |   |   ├── assign_test.sy
    |   │   ├── fun_test.sy
    |   │   ├── if_test.sy
    |   │   ├── io.sy
    |   │   ├── io.in
    |   │   ├── io.out
    |   │   ├── qsort.sy                <- 需要提交的qsort的SysYF源程序文件（第2关）
    |   │   ├── qsort.in                <- 需要提交的qsort的输入数据文件（第2关）
    |   │   ├── qsort.out               <- 需要提交的qsort的输出结果文件（第2关）
    |   │   └── while_test.sy
    |   └── demo
    |       |── CMakeLists.txt
    |       |── go_upstairs.sy
    |       └── go_upstairs_gen.cpp     <- 打印go_upstairs.ll的cpp文件
    └── task3							<- 第3关任务相关的目录
        ├─ CMakeLists.txt          
        ├─ main.cpp                     <- 调用编译器驱动程序的主程序文件，需要提交对qsort程序的编译执行版本（第3关）
        ├─ include
        |	├─ IR
        |   |   ├─ gcd_generator.hpp  	
        |   |   └─ qs_generator.hpp   	<- 需要提交的qsort程序的LLVM IR生成器（第3关）
        |   ├─ runtime
        |   |   ├─ io.h  				
        |   |   └─ runtime.h   		
        |   └─ Driver                  
        ├─ src                        
        |   ├─ Checker
        |   └─ Driver
        └─ tests                       
            ├─ quicksort.c 				第3关待翻译为LLVM IR的目标c语言程序
            ├─ quicksort_[*].in         第3关测试样例输入
            └─ quicksort_[*].out        第3关测试样例输出
```

### 1.2 提交要求和评分标准
* 提交要求  
  本实验的提交要求分为两部分: 实验部分的文件和实验报告。
  * 实验部分
    * 需要完成 `./Student/task1/ll`目录下的5个文件
    * 需要完成 `./Student/task2/cpp`目录下的6个`.cpp`文件和`./Student/task2/sy`目录下的3个文件`qsort.in`、`qsort.sy`、`qsort.out`
    * 需要完成`./Student/task3/main.cpp`以及`./Student/task3/include/qs_generator.hpp`两个文件，并保持与`./Student/task3/tests`中的测试样例相同的输入输出格式
    * 需要在 `./report/report.md` 中撰写实验报告
      * 实验报告内容包括:
        * 实验要求、问题回答、实验设计、实验难点及解决方案、实验总结、实验反馈(具体参考[report.md](./report.md))
  * 提交规范: 
    * 不破坏目录结构(`report.md`如果需要放图片，请新建`figs`文件夹放在`./report`下，并将图片放在`figs`文件夹内)
    * 不上传临时文件(凡是自动生成的文件和临时文件请不要上传)
* 评分标准: 本次实验分为3部分
  * **禁止执行恶意代码，违者本次实验0分处理**
  * 第一部分12分: `.ll`运行结果正确(1个2分, 注释共2分)
  * 第二部分30分: `.cpp`运行结果正确(1个5分)
  * 第三部分30分: `mClang -IR`命令的运行结果正确
* 迟交规定
  * 迟交需要邮件通知助教: 
    * 邮件主题: IRLab迟交-学号
    * 内容: 包括迟交原因、最后版本commit ID、迟交时间等
* 关于抄袭和雷同
  经过助教和老师判定属于作业抄袭或雷同情况，所有参与方一律零分，不接受任何解释和反驳。

如有任何问题，欢迎提issue进行批判指正。
