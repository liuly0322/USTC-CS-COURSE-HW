# LLVM驱动程序教程

本目录提供教你用Clang/LLVM应用编程接口构建编译器、程序分析器或优化器的示例代码。我们定义了可扩展的`Driver`类，它调用Clang/LLVM API实现对输入的源程序文件的解析，产生AST、LLVM IR等中间表示，并在这些中间表示上开展程序分析和变换。

## 目录说明

```
/
 ├─ CMakeLists.txt          
 ├─ main.cpp                    入口程序,调用Driver，需要理解并修改
 ├─ include
 |	 ├─ IR
 |   |   ├─ gcd_generator.h  	使用LLVM IR构建求解最大公因数的程序
 |   |   └─ qs_generator.h   	使用LLVM IR构建快速排序算法的程序, 需要修改
 |   ├─ runtime
 |   |   ├─ io.h  				输入输出函数头文件
 |   |   └─ runtime.h   		LLVM IR Module中的输入输出和计时函数对象
 |   └─ Driver                  封装的Driver类的定义
 ├─ src                         函数定义
 |   ├─ runtime					
 |   └─ Driver
 └─ tests                       测试样例
 	 ├─ quicksort.c 			待翻译为LLVM IR的目标c语言程序
 	 ├─ quicksort_[*].in        测试样例输入
 	 └─ quicksort_[*].out       测试样例输出
```

## 驱动程序支持的功能

如果输入c语言文件：

1. 支持打印Clang生成的AST;
2. 调用[Clang Driver](https://github.com/llvm/llvm-project/blob/release/11.x/clang/lib/Driver/Driver.cpp)产生LLVM IR；
3. 支持参数解析、定制`Pass`并自动打印`LLVM IR`；

输入c语言文件或者调用返回`IR Module`:

1. 调用LLVM IR层次的一些TransForm Passes，对IR进行优化；
2. 基于`DynamicLibrary`机制，支持运行时输入输出；
3. 基于`MCJIT`模块，支持即时编译执行。

## 使用说明

### 1. 编译Clang和LLVM

我们已经为大家准备了实验环境；如果你打算在本机配置相同的实验环境的话，可以参考[llvm 11.0.0安装文档](./docs/LLVM-11.0.0-install.md)。

### 2. 使用Driver

在本软件包根目录下，执行下面的命令编译驱动程序：

```bash
mkdir build && cd build
cmake .. # for relaese
# cmake .. -DCMAKE_BUILD_TYPE=Debug # for debug, e.g. using gdb to trace
make [-j<num>]
```

生成的可执行文件为`build/mClang`，mClang的使用方法及参数可以通过执行下面命令获知。

```bash
./mClang -h
```

例如，可以通过下面的命令，解析`quicksort.c`程序并执行输出以及计时：

```shell
cat ../tests/quicksort_1024.in | ./mClang ../tests/quicksort.c -I ../include/
```

## 研制者

本驱动软件由中国科学技术大学[张昱](http://staff.ustc.edu.cn/~yuzhang/)老师[团队](https://s4plus.ustc.edu.cn/)研制，研制者包括刘硕、黄奕桐、王顺洪、廖美慧等。徐伟老师参与算法理解和环境搭建等工作。
