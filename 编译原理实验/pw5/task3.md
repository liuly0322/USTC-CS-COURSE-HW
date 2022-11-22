## 任务说明

本关要求在上一关的基础上进一步修改语义检查器`SyntaxTreeChecker`的源文件`include/SyntaxTreeChecker.h`和`src/SyntaxTreeChecker.cpp`，实现如下功能
1. 函数调用必须保证实际参数的**个数**和**类型**都与函数声明中的形式参数**完全匹配**。如果输入程序存在函数实参和形参不匹配的情况，则返回 `ErrorType::FuncParams`
2. 检查函数是否未定义以及是否重复定义。如果输入程序存在未定义或重复定义，则分别返回 `ErrorType::FuncUnknown` 和 `ErrorType::FuncDuplicated` 

注意：**不能**修改 `include/SyntaxTree.h`

## 实验引导
### 函数声明与使用的语义检查

`SysYF` 语言的函数调用必须保证实际参数的**个数**和**类型**都与函数声明中的形式参数**完全匹配**，并且不能存在所调用的函数未定义以及函数重复定义的情况。

本实验无需考虑函数的实际参数为一个没有返回值（类型为 `void`）的函数，即函数传入的参数要么为 `int`，要么为 `float` ，特别的，`float` 和 `int` 运算的结果为 `float` 。

### 其它注意事项
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

## 思考题

请在 `doc/answer.md` 中详细回答下述思考题。

#### 问题3-1.

请提交一份实验总结，该实验总结必须包含以下内容：
1. 你在实验过程中遇到的困难与解决方案
2. 你认为的实验难点与考察倾向
3. 你的整体实现思路与实现亮点

#### 问题3-2.

试回答：处理变量声明时，是应该先处理变量的初始化还是先把该变量加入符号表？处理函数定义时，是先处理函数体还是先把函数名加入符号表？

#### 问题3-3.

有人说如果本次实验允许修改抽象语法树的话，他想给 `Expr` 添加一个属性用于判断是否为整数，而不是在 `SyntaxTreeChecker` 内部设置该属性。这种做法有什么缺点？

另一个人说，完成本次实验后 `SyntaxTreeChecker` 类过于庞大了，他想拆成多个类，每个类处理一种可能的语义错误，这种做法又有什么缺点？

#### 问题3-4.

如果想给错误人为地规定一个优先级，例如，如果取模运算是最高优先级，那么语义检查器在遇到存在取模运算错误的程序时，必定报取模运算错误，也即`ErrorType::Module`。对于如下程序：
```cpp
int f(int x)
{
	return 0;
}
int main(){
	int a = f(0.1);
	return a % 0.1;
}
```
将报错`ErrorType::Module`，试谈一下你的实现思路。