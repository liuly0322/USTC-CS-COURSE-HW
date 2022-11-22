## 任务说明
`sysYFParser`解析器（参见`src/main.cpp`中的`driver.parse(filename)`）只能在输入程序`filename`不符合语法规则时报错，但难以检测输入程序中可能存在的语义错误。例如，当输入程序中存在如下取模运算的操作数不是整数时：
```
int a = 1 % 0.1;
```
`sysYFParser` 不会报错，但对浮点数取模是没有意义的。

在本任务中，我们首先提供一个[初始语义检查器](#初始语义检查器)，用于检查取模运算所作用的常量操作数是否合法，该语义检查器可以使用`./sysYFParser xxxx.sy`命令运行；你需要进一步修改语义检查器的源文件`include/SyntaxTreeChecker.h`和`src/SyntaxTreeChecker.cpp`，以扩展增加如下语义检查功能：
1. 能够检查程序中任何地方出现的取模运算的操作数是否有效
2. 检查变量是否未定义而使用以及变量在同一作用域下是否重复定义

注意：在本关任务中，你**不能**修改 `include/SyntaxTree.h`。

## 实验引导
### 初始语义检查器

与 `SyntaxTreePrinter` 类似，`SyntaxTreeChecker` 也是通过遍历抽象语法树来实现对输入程序的语义分析，初始语义检查器已经实现了对于取模运算的操作数是否为整数的**部分检查**，具体情况如下：

1. 初始语义检查器实现了`Assembly`节点的`visit`函数，它通过遍历全局变量的初始化节点来对其进行检查

```cpp
void SyntaxTreeChecker::visit(Assembly& node) {
    for (auto def : node.global_defs) {
        def->accept(*this);
    }
}
```
2. 取模运算的语义检查操作位于`BinaryExpr`节点的`visit`函数中
	- 首先借助访问者模式访问左右操作数，取得`lhs_int`与`rhs_int`
    - 其次在双目运算符为取模时，插入对左右操作数类型的判断，即判断其是否为整型

```cpp
void SyntaxTreeChecker::visit(BinaryExpr& node) {
    node.lhs->accept(*this);
    bool lhs_int = this->Expr_int;
    node.rhs->accept(*this);
    bool rhs_int = this->Expr_int;
    if (node.op == SyntaxTree::BinOp::MODULO) {
        if (!lhs_int || !rhs_int) {
            err.error(node.loc, "Operands of modulo should be integers.");
            exit(int(ErrorType::Modulo));
        }
    }
    this->Expr_int = lhs_int & rhs_int;
}
```

### 对任何情况下的取模运算进行语义检查
对于初始语义检查器，容易发现其存在有很大缺陷，例如：
- 只能检测全局变量初始化时的操作数是否为整数，无法处理函数块内部声明的变量的情况：
```
int main() {
    int a = 1 % 0.1;
}
```
- 只能处理操作数为字面量的情况，如果操作数是一个已经声明为 `float` 类型的变量或返回值不为 `int` 的函数调用（这里假设函数声明时的类型与其实际返回的类型相同），也应该报错：
```
int main() {
    float a = 0.1;
    int b = 1 % a;
}
```
你需要改进`SyntaxTreeChecker` 的实现，使之能够检查**任何情况**下的取模运算的操作数是否有效，包括但不限于上述两类情况。

你可以先假设不会出现变量和函数未定义或重复定义的情况，且所有表达式均不含没有返回值的函数。

### 处理当前作用域内变量未定义和重复定义的情况

在完成上一部分之后，你还需要解决两个重要问题：变量可能未定义，以及变量可能重复定义。

你需要进一步修改`SyntaxTreeChecker` 的实现，以完成对**当前作用域**内变量是否未定义和重复定义的检查，并仿照示例，在识别出这样的错误时返回 `ErrorType::VarUnknown` 和 `ErrorType::VarDuplicated`。

注意：**本实验无需考虑函数名与变量名相同的情况。**

**作用域**可以参考维基百科中对C语言作用域的定义，如下：
`An identifier can denote an object; a function; a tag or a member of a structure, union, or enumeration; a typedef name; a label name; a macro name; or a macro parameter. The same identifier can denote different entities at different points in the program. For each different entity that an identifier designates, the identifier is visible (i.e., can be used) only within a region of program text called its scope.`

**提醒**：1）形式参数在函数体的顶层作用域内；2）参考教材 7.2.2 节作用域信息的保存。

## 其它注意事项
本实验所用到的错误类型如下：
```cpp
enum class ErrorType {
    Accepted = 0,
    Modulo,
    VarUnknown,
    VarDuplicated,
    FuncUnknown,
    FuncDuplicated,
    FuncParams
};
```

本关测试集共 20 个样例，前 10 个样例不考虑变量未定义和重复定义，只处理取模问题；后 10 个样例考虑变量定义问题。