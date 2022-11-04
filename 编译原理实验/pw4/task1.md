# 通过demo熟悉Flex和Bison的使用

- [通过demo熟悉Flex和Bison的使用](#通过demo熟悉flex和bison的使用)
  - [任务描述](#任务描述)
  - [目录结构](#目录结构)
  - [demo简介](#demo简介)
  - [环境安装](#环境安装)
  - [demo使用说明](#demo使用说明)
    - [如何构建](#如何构建)
    - [如何使用](#如何使用)
  - [demo语言说明](#demo语言说明)
  - [demo语言的词法描述](#demo语言的词法描述)
    - [Flex简介](#flex简介)
    - [demo词法说明](#demo词法说明)
  - [demo语法描述](#demo语法描述)
    - [Bison简介](#bison简介)
    - [demo语法说明](#demo语法说明)
  - [demo词法、语法交互](#demo词法语法交互)
    - [问题1-1](#问题1-1)
    - [Bison对token生成的对应函数](#bison对token生成的对应函数)
    - [问题1-2](#问题1-2)

## 任务描述

在本关，你需要理解demo的工作原理并[运行使用demo](#demo使用说明)，同时在`doc/answer.md`中回答[问题1-1](#问题1-1)和[问题1-2](#问题1-2)

## 目录结构
```
demo
├── CMakeLists.txt
├── README.md
├── grammar
│   ├── demoParser.yy         # demo语言的语法描述
│   ├── demoScanner.ll        # demo语言的词法描述
│   └── test.sy               # 简单测试样例
├── include
│   ├── demoDriver.h          # demo语言前端的驱动器，驱动词法语法分析器工作
│   ├── demoFlexLexer.h       # demo语言的词法分析器包装类
│   ├── ErrorReporter.h       # 错误处理相关类的声明
│   ├── SyntaxTree.h          # 语法树节点的定义
│   └── SyntaxTreePrinter.h   # 根据语法树打印源代码
└── src
    ├── demoDriver.cpp         # demo语言前端的驱动器，驱动词法语法分析器工作
    ├── ErrorReporter.cpp      # 错误处理相关类的声明
    ├── main.cpp               # 主函数所在
    ├── SyntaxTree.cpp         # 语法树节点的方法实现
    └── SyntaxTreePrinter.cpp  # 根据语法树打印源代码
```

## demo简介

[demo](#demo语言说明)是一个简单的C语言子集的前端。使用flex构建词法分析器、bison构建语法分析器。在语法分析的同时构建抽象语法树(AST)。

## 环境安装

正确使用本项目需要安装`flex`以及`bison 3.7.6`及以上。实验环境已经配置好了`bison 3.7.6`和`flex 2.6.4`。下面说明如何在本地安装

这里以ubuntu和archlinux为例说明环境如何安装。

`flex`安装：

Ubuntu:

```shell
$ sudo apt update
$ sudo apt install flex
```

Arch Linux:

```shell
$ sudo pacman -Syyy
$ sudo pacman -S flex
```

`bison`安装:

Ubuntu:

Ubuntu由于通过apt只能安装bison 3.5版本，所以需要手动安装，步骤如下：（**注**:若已经安装了老版本的bison,**请在安装前卸载掉**，卸载命令可以使用`sudo apt remove bison`）

```shell
$ wget http://ftp.gnu.org/gnu/bison/bison-3.7.6.tar.gz
$ tar xvzf bison-3.7.6.tar.gz
$ cd bison-3.7.6
$ sudo ./configure
$ sudo make
$ sudo make install
```

安装完成后使用`bison --version`，若出现如下类似的输出，则安装成功

```shell
$ bison --version
bison (GNU Bison) 3.7.6
由 Robert Corbett 和 Richard Stallman 编写。

版权所有 (C) 2021 Free Software Foundation, Inc.
这是自由软件；请参考源代码的版权声明。本软件不提供任何保证，甚至不会包括
可售性或适用于任何特定目的的保证。
```

Arch Linux:

```shell
$ sudo pacman -Syyy
$ sudo pacman -S bison
```

## demo使用说明

### 如何构建
在demo目录下执行以下指令

```shell
$ mkdir build; cd build
$ cmake ..
$ make
```
此命令执行后将会在新建的build目录下生成可执行文件demo，此文件的使用方法见下

```shell
$ ./demo -h
Usage: ./demo [ -h | --help ] [ -p | --trace_parsing ] [ -s | --trace_scanning ] [ -emit-ast ] <input-file>
$
```

### 如何使用
demo的命令行各个flag的使用说明:

- `-h`或`--help` 打印帮助信息后退出

- `-p`或`--trace_parsing` 输出语法分析详情

- `-s`或`--trace_scanning` 输出词法分析详情

- `-emit-ast` 通过AST（Abstract Syntax Tree）复原代码，并输出

- 当`input-file`为`-`时，将输入流切换为标准输入

以`demo/grammar/test.sy`为例：

```c
void main(){
    int a = 1, c;
    int b = 0777;
    b = a * c - a;
    {a = b + a;}
    b = -b++c%a;
    return;
}

```

使用如下shell命令：
```shell
$ mkdir build; cd build
$ cmake ..
$ make
$ ./demo -emit-ast ../grammar/test.sy
```
输出如下:
```
void main()
{
    int a = 1;
    int c;
    int b = 511;
    b = ((a*c)-a);
    {
        a = (b+a);
    }
    b = ((-b)+((+c)%a));
    return;
}
```
输出内容与demo命令行指定的输入源文件的内容是等价的。

## demo语言说明
demo是一个简单的编译器前端示例，它支持的语言是一个C语言的有限子集，其特点有：
- 不支持全局变量，源程序文件里面只可以包含一个函数定义，函数只能是void返回类型且不可以有参数；
- 局部变量只能是int类型，不支持数组，局部变量可以初始化，支持十进制、八进制；
- 不支持任何改变控制流的语句，即分支与循环；
- 允许函数定义内部使用语句块；
- 返回语句不是必需的，返回语句不能带有返回值；
- 支持四则运算、取模运算、取负号以及取正号运算符，不支持任何逻辑运算符；
- 语句可以是赋值语句、表达式语句、空语句、返回语句。

该语言的文法采用EBNF(Extended Backus-Naur Form)形式化地表示如下：

```
CompUnit    → FuncDef
FuncDef     → FuncType Ident '(' ')' Block
FuncType    → 'void'
Block       → '{' { BlockItem } '}'
BlockItem   → VarDecl | Stmt
VarDecl     → BType VarDef { ',' VarDef } ';'
BType       → 'int'
VarDef      → Ident | Ident '=' Exp
Stmt        → LVal '=' Exp ';'
            | Exp ';'
            | 'return' ';'
            | Block
            | ';'
LVal        → Ident
Exp         → Exp BinOp Exp
            | UnaryOp Exp
            | '(' Exp ')'
            | LVal
            | Number
Number      → IntConst
BinOp       → '+' | '-' | '*' | '/' | '%'
UnaryOp     → '+' | '-'
```

## demo语言的词法描述

demo的词法分析器使用Flex工具构建。

### Flex简介

[Flex](https://github.com/westes/flex)(Fast lexical analyzer generator)是词法分析器生成器lex的变种，它接受词法描述文件，产生对应的词法分析器的 C 源代码。

demo的词法描述文件见`demo/grammar/demoScanner.ll`,下面依次解释该文件由`%%`分隔的三段内容。

### demo词法说明

1. 声明部分

`demo/grammar/demoScanner.ll`中第1个%%之前的部分是声明，其中%{...%}括起的代码会被Flex直接复制到生成的词法分析器的 C 源码中，但不检查其合法性:

```c
%{ /* -*- C++ -*- */
# include <cerrno>
# include <climits>
# include <cstdlib>
# include <cstring> // strerror
# include <string>
# include <algorithm>
# include <iostream>
# include <map>
# include "demoDriver.h"
# include "demoParser.h"
%}

...
```

同时也可包含词法符号的定义

```flex
Identifier          [_a-zA-Z][a-zA-Z0-9_]*
OctConst            ("0"[0-7]*)
DecConst            ([1-9][0-9]*)
Blank               [ \t\r]
NewLine             [\n]
```

2. 规则部分
在词法描述文件中，介于两个%%之间的是词法规则及动作部分。`demo/grammar/demoScanner.ll`中的词法规则及动作如下：

**注：此时可以先忽略{}中的具体C++代码。后续会有相关说明**

```flex
 /* keyword */
int         {return yy::demoParser::make_INT(loc);}
return      {return yy::demoParser::make_RETURN(loc);}
void        {return yy::demoParser::make_VOID(loc);}

[+]         {return yy::demoParser::make_PLUS(loc);}
[-]         {return yy::demoParser::make_MINUS(loc);}
[*]         {return yy::demoParser::make_MULTIPLY(loc);}
[/]         {return yy::demoParser::make_DIVIDE(loc);}
[%]         {return yy::demoParser::make_MODULO(loc);}
[=]         {return yy::demoParser::make_ASSIGN(loc);}
[;]         {return yy::demoParser::make_SEMICOLON(loc);}
[,]         {return yy::demoParser::make_COMMA(loc);}
[(]         {return yy::demoParser::make_LPARENTHESE(loc);}
[)]         {return yy::demoParser::make_RPARENTHESE(loc);}
[{]         {return yy::demoParser::make_LBRACE(loc);}
[}]         {return yy::demoParser::make_RBRACE(loc);}


{Blank}+                {loc.step();}
{NewLine}+              {loc.lines(yyleng); loc.step();}
{DecConst}              {
                            std::string dec = yytext;
                            unsigned sum = 0;
                            int len = dec.size();
                            for(int i = 0;i < len;i++){
                                auto a = dec[i];
                                sum = sum * 10 + a - 48;
                            }
                            return yy::demoParser::make_INTCONST(int(sum),loc);
                        }
{OctConst}              {
                            std::string oct = yytext;
                            unsigned sum = 0;
                            int len = oct.size();
                            for(int i = 1;i < len;i++){
                                auto a = oct[i];
                                sum  = sum * 8 + a - 48;
                            }
                            return yy::demoParser::make_INTCONST(int(sum),loc);
                        }
{Identifier}            {return yy::demoParser::make_IDENTIFIER(yytext, loc);}
<<EOF>>                 {return yy::demoParser::make_END(loc);}
.                       {std::cout << "Error in scanner!" << '\n'; exit(1);}
```

3. 代码部分
在词法描述文件的第2个%%之后是C代码部分，这部分代码将被Flex直接复制到生成的词法分析器源码的尾部。

综上，一个完整的一个完整的flex输入文件的格式为：
```flex
%{
Declarations
%}
Definitions
%%
Rules
%%
User subroutines
```

- 输入文件的第 1 段 %{ 和 %} 之间的为 声明（Declarations） ，都是 C 代码，这些代码会被复制到生成的词法分析器的 C 源码中，一般在这里声明一些全局变量和函数等，这样在后面可以使用这些变量和函数。

- 第 2 段 %} 和 %% 之间的为 定义（Definitions），在这里可以定义正则表达式中的一些名字，可以在 规则（Rules） 段被使用，如`demo/grammar/demoScanner.ll`中定义了 `DecConst` 为 `([1-9][0-9]*)` ， 这样在后面可以用 `{DecConst}` 代替这个正则表达式。

- 第 3 段为 规则（Rules） 段。

- 第 4 段为 用户定义过程（User subroutines） 段，也都是 C 代码，本段内容会被原样复制到生成的词法分析器最末尾，一般在此定义第 1 段中声明的函数。

以上 4 段中，除了 Rules 段是必须要有的外，其他三个段都是可选的。

**Tips**
1. 当有多条规则的模式被匹配到时，会选择匹配长度最长的那条规则；如果有匹配长度相等的规则，则选择排在最前面的规则。

2. Flex生成的词法分析器中有许多预置的全局变量，其中`yytext`和`yyleng`表示刚刚匹配到的字符串以及它的长度。

3. 更多内容可以参考[https://github.com/westes/flex](https://github.com/westes/flex)

## demo语法描述

demo的语法分析部分使用Bison工具构建。

### Bison简介

[Bison](https://www.gnu.org/software/bison/)是YACC（Yet another compiler compiler）的变种，YACC以LALR(1)分析技术为基础。

- Bison 支持的分析表构造算法
  
  由于历史原因，Bison缺省地构造LALR(1)分析表，但LALR不能处理所有的LR文法。不过，针对非LALR文法在冲突时可能发生的不确定行为，Bison提供简单的方法来解决冲突。你只需要使用%define lr.type \<type\>指令来激活功能更强大的分析表构造算法，其中参数\<type\>缺省地为lalr，还可以为ielr(Inadequacy Elimination LR）或canonical-lr。有关Bison的LR分析表构造的说明详见[这里](https://www.gnu.org/software/bison/manual/html_node/LR-Table-Construction.html)。在本实验中，仍以LALR分析表构造算法为基础。

- Bison 支持的文法
  
  Bison允许输入的文法是二义的(ambiguous)，或者非二义但是是不确定的(nondeterministic)，即没有固定向前看(lookahead)的记号数使得能明确要应用的文法规则。
  通过引入适当的声明（如%glr-parser、%expect n、%expect-rr n、%edprec、%merge），Bison能够使用GLR(Generalized LR)分析技术来分析处理更一般的上下文无关文法，对这些文法而言，任何给定字符串的可能解析次数都是有限的。Bison的GLR分析参见[这里](https://www.gnu.org/software/bison/manual/html_node/Generalized-LR-Parsing.html)。

- 冲突及冲突的解决
  
  对于某些文法，按Bison的分析表构造算法构造出的分析表存在有不确定的单元项（某个单元格有不止一种动作选项），这时称为存在移进-归约冲突([Shift/Reduce Conflicts](https://www.gnu.org/software/bison/manual/html_node/Shift_002fReduce.html))或者归约-归约冲突([Reduce/Reduce Conflicts](https://www.gnu.org/software/bison/manual/html_node/Reduce_002fReduce.html))。当文法存在冲突时，你需要了解Bison对冲突采取的处理策略，或者尝试修改文法以减少冲突，或者让Bison自动处理冲突，但是你需要知道Bison的自动处理策略。

- 移进-归约冲突的解决
  
  为避免Bison发出移进-归约冲突的警告，可以使用%expect n声明，只要移进-归约冲突的数量不超过n，Bison就不报这些警告（参见[这里](https://www.gnu.org/software/bison/manual/html_node/Expect-Decl.html)）。Bison会按**优先移进**来解决这种冲突。不过，一般不建议使用%expect声明（%expect 0除外），而是建议使用优先（%precedence）和结合性（%left、%right、%noassoc）的指令去显式声明记号的优先级和结合性，或者进一步对某条文法规则用%prec <symbol>声明按给定符号<symbol>的优先级和结合性来处理该条规则。有关Bison提供显式修复的优先级和结合性声明方法，参见[对非算子使用优先级](https://www.gnu.org/software/bison/manual/html_node/Non-Operators.html)、[算子优先级](https://www.gnu.org/software/bison/manual/html_node/Precedence.html)。

- 归约-归约冲突的解决
  
  归约-归约冲突发生在对输入的相同序列可以应用2条或多条文法规则的时候。可以在使用GLR分析技术时，用%expect-rr n声明指定期望的归约-归约冲突的次数。Bison会按**选择文法中首先出现的规则**来解决在期望范围内的冲突。同样地，也不建议使用%expect-rr n，而是希望用优先级和结合性指令来显式消除。

### demo语法说明

1. 声明部分

`demo/grammar/demoParser.yy`中第1个%%之前的部分是声明。

%{...%}括起的代码会被Bison直接复制到生成的语法分析器的 C 源码中，但不检查其合法性。这部分主要将要用到的头文件包含进来。

**记号的声明**

%token用来声明记号（终结符）的种类及语义值类型（可以空缺）。有关Bison的记号声明详见[这里](https://www.gnu.org/software/bison/manual/html_node/Union-Decl.html)。

`demo/grammar/demoParser.yy`中的token声明：

```bison
%token END
%token PLUS MINUS MULTIPLY DIVIDE MODULO
%token ASSIGN SEMICOLON
%token COMMA LPARENTHESE RPARENTHESE
%token LBRACE RBRACE
%token INT RETURN VOID
%token <std::string>IDENTIFIER
%token <int>INTCONST
%token EOL COMMENT
%token BLANK NOT
```

上面`INTCONST`语义值类型为`int`，`IDENTIFIER`语义值类型为`std::string`。

**优先级与结合性的声明**

%left、%right、%nonassoc用来声明记号的结合性，依次为自左向右的结合、自右向左的结合和无结合性。%precedence用来声明优先级而没有结合性。记号的优先级是由记号结合性声明所在行的先后次序来决定的，先出现的优先级较低；在同一行声明的记号具有相同的优先级。
在下面3行优先级和结合性声明中，负号UMINUS和正号UPlUS的优先级最高(见`demo/grammar/demoParser.yy`212行)。有关Bison的优先级声明详见[这里](https://www.gnu.org/software/bison/manual/html_node/Precedence-Decl.html)。

```bison
%left PLUS MINUS;
%left MULTIPLY DIVIDE MODULO;
%precedence UPLUS UMINUS;
```

**文法符号的语义值类型声明**

在早期的Bison版本中，%type用来声明终结符和非终结符的语义值类型；但是在后来的Bison版本中，只能用%type和%nterm声明非终结符的语义值类型。有关Bison的文法符号的语义值类型声明详见[这里](https://www.gnu.org/software/bison/manual/html_node/Type-Decl.html)。

`demo/grammar/demoParser.yy`中的%type声明：

```bison
%type <SyntaxTree::Assembly*>CompUnit
%type <SyntaxTree::PtrList<SyntaxTree::GlobalDef>>GlobalDecl
%type <SyntaxTree::Type>BType
%type <SyntaxTree::PtrList<SyntaxTree::VarDef>>VarDecl
%type <SyntaxTree::PtrList<SyntaxTree::VarDef>>VarDefList
%type <SyntaxTree::VarDef*>VarDef
%type <SyntaxTree::InitVal*>InitVal
%type <SyntaxTree::FuncDef*>FuncDef
%type <SyntaxTree::BlockStmt*>Block
%type <SyntaxTree::PtrList<SyntaxTree::Stmt>>BlockItemList
%type <SyntaxTree::PtrList<SyntaxTree::Stmt>>BlockItem
%type <SyntaxTree::Stmt*>Stmt
%type <SyntaxTree::LVal*>LVal
%type <SyntaxTree::Expr*>Exp
%type <SyntaxTree::Literal*>Number
```

**开始声明**

%start 可以用来指明语法规则的开始。在`demo/grammar/demoParser.yy`中：

```bison
%start Begin
```

指明语法分析从文法符号`Begin`开始

2. 规则部分

在语法描述文件中介于两个%%之间的是文法规则部分。`demo/grammar/demoParser.yy`中的部分文法规则如下：

```bison
VarDef:IDENTIFIER{
    $$=new SyntaxTree::VarDef();
    $$->name=$1;
    $$->is_inited = false;
    $$->loc = @$;
  }
	| IDENTIFIER ASSIGN InitVal{
    $$ = new SyntaxTree::VarDef();
    $$->name = $1;
    $$->initializers = SyntaxTree::Ptr<SyntaxTree::InitVal>($3);
    $$->is_inited = true;
    $$->loc = @$;
  }
;
```

**文法规则**

每条规则以分号结尾，冒号左边的符号是要定义的非终结符，冒号右边的是文法符号和动作组成的序列。有关Bison的文法规则说明详见[这里](https://www.gnu.org/software/bison/manual/html_node/Rules-Syntax.html)。

**动作**

动作由花括号括起的C代码组成，可以放在文法规则右端（right-hand side）的任意位置。绝大多数的规则只包含置于规则尾部的动作，如上面`demo/grammar/demoParser.yy`中的规则。有关Bison的动作说明详见[这里](https://www.gnu.org/software/bison/manual/html_node/Actions.html)。

动作中的C代码可以通过`$n`引用与该规则匹配的第n个组成元素的语义值。`$$`表示该规则正在定义的文法符号的语义值。文法符号的语义值还可以用名字来访问，如`$name`或`$[name]`。也可以通过`@$`来引用当前正在定义的文法符号的位置。

以上面展出的`demo/grammar/demoParser.yy`中的规则为例，`$$`表示`VarDef`的语义值，`$1`表示`IDENTIFIER`的语义值，`$3`表示`InitVal`的语义值。`@$`表示`VarDef`的位置。

**上下文相关的优先级**

在文法规则部分，可以使用%prec声明所作用的规则的优先级为所指定的符号的优先级。通过这种声明，可以解决负号与减号文本相同但含义不同的冲突问题。有关Bison的上下文相关的优先级说明详见[这里](https://www.gnu.org/software/bison/manual/html_node/Contextual-Precedence.html)。

**注意**：负号和减号的冲突是在语法分析阶段解决的，而词法分析阶段对`-`识别后返回的都是记号MINUS。

在`demo/grammar/demoParser.yy`中存在如下代码片段(212行开始):

```bison
%left PLUS MINUS;
%left MULTIPLY DIVIDE MODULO;
%precedence UPLUS UMINUS;

Exp:PLUS Exp %prec UPLUS{
    auto temp = new SyntaxTree::UnaryExpr();
    temp->op = SyntaxTree::UnaryOp::PLUS;
    temp->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>($2);
    $$ = temp;
    $$->loc = @$;
  }
  ...
```

其中：声明部分引入一个新的符号`UPLUS`用来声明负号的优先级；然后在文法规则部分，对PLUS Exp增加声明%prec UPLUS，指出该表达式按UPLUS的优先级处理，这样通过bison生成的分析器就能正确识别和处理负号和减号的不同语义。

3. 代码部分

在文法描述文件中第2个%%之后是C代码部分，例如在`demo/grammar/demoParser.yy`中：

```cpp
void yy::demoParser::error (const location_type& l,
                          const std::string& m)
{
    driver.error(l, m);
}
```

Bison约定：由其生成的分析器会调用名字为yyerror()的函数来报告错误，而这个函数需要由用户提供定义。

函数yyparse()是Bison生成的分析器的总控函数。

## demo词法、语法交互

本部分将简述demo中词法和语法是如何交互的

使用`make`命令后。在build目录下会产生`demoParser.h`、`demoScanner.cpp`、`demoParser.cpp`、`location.hh`、`position.hh`、`stack.hh`文件。这是Flex和Bison自动生成的词法、语法分析源代码。其中`demoScanner.cpp`是flex生成的词法文件，其余文件是通过`bison`生成。

在`demo`中`bison`与`flex`的交互通过`demoParser.h`完成。在`demoScanner.ll`第11行可以看到如下代码:
```c
# include "demoDriver.h"
# include "demoParser.h"
```
`demoDriver.h`和`demoDriver.cpp`驱动两者共同完成工作。

### 问题1-1
阅读`demo`中的`main.cpp`、`demoDriver.cpp`、`demoDriver.h`，简要说明`Driver`是如何工作的。

### Bison对token生成的对应函数

Bison对于在语法描述文件中声明的每个token，会在.h文件中生成对应的make函数。如`demo/grammar/demoParser.yy`在第48行声明了`%token INT`。则bison会在自动生成的`demoParser.h`中生成一个`make_INT`函数(1409行)，其接受一个location参数，返回给语法分析器一个`INT`token。在`demoScanner.ll`中可以看到如下代码:

```flex
int         {return yy::demoParser::make_INT(loc);}
```

`demo/grammar/demoParser.yy`在第50行声明了`%token <int>INTCONST`。该token与上面的INT不同，多了类型`int`。其在`demoParser.h`中的声明如下：
```c
static symbol_type make_INTCONST (const int& v, const location_type& l)
{
        return symbol_type (token::TOK_INTCONST, v, l);
}
```
可以发现其与`%token INT`生成的函数相比，参数里多了int 参数。可以看出，对于声明有类型的token，bison在生成的make_XXX函数参数中会有对应的类型。其在`demoScannser.ll`中的一个使用如下：
```flex
{DecConst}              {
                            std::string dec = yytext;
                            unsigned sum = 0;
                            int len = dec.size();
                            for(int i = 0;i < len;i++){
                                auto a = dec[i];
                                sum = sum * 10 + a - 48;
                            }
                            return yy::demoParser::make_INTCONST(int(sum),loc);
                        }
```
其中yytext表示当前flex词法分析器匹配到的串。在当前场景下表示匹配到的10进制数字串。`{}`中的c++代码用于将10进制数字串转换为对应的数字，功能类似atoi()函数。

### 问题1-2
阅读并理解demo代码以及Bison-Flex协作的方式，目前的实现中并不强制demo语言程序包含main函数，而这不符合文档描述的语言规范。若在前端分析过程中就强制要求demo语言程序必须包含且只能包含一个main函数（也就是说把这个约束用语法定义来体现），应该如何修改词法语法定义？
