# PW5 Problems

## 1.1

> 请阅读 `include/SyntaxTreePrinter.h` 和 `src/SyntaxTreePrinter.cpp`，总结 `VarDef`、`InitVal`、`LVal`、`FuncDef`、`FuncFParamList`、`FuncParam`、`BlockStmt` 等节点类的结构及其 `visit` 方法的实现特点。

结构：除了继承基类之外，类成员还包含自己的独有成员，包含该结点的相关信息（例如 `VarDef` 包含该变量定义（声明）是否声明 const 等），或者指向其他结点的指针或指针数组

实现特点：对语法树的遍历是通过从语法树的根结点开始递归实现的，对每个结点的 `visit` 方法会按照 SysYF 语法打印结点成员（即：每个语法树结点类都有对应接收自己的 `visit` 方法），其中如果涉及到指向的别的结点，会调用别的结点的 `accept` 方法，接收 `visitor`

## 3-1

> 请提交一份实验总结，该实验总结必须包含以下内容：
>
> 1.  你在实验过程中遇到的困难与解决方案
> 2.  你认为的实验难点与考察倾向
> 3.  你的整体实现思路与实现亮点

### 困难与解决

#### 函数形参类型的储存

由于 task2 的要求并不高，我采用的是维护一个符号表（`unordered_map`）的方式，保存符号及其类型，对于函数，直接以它的返回值作为该符号的类型。

在此基础上，为了实现函数形参类型的保存以便后续比较，大致构想了以下几种方案：

- 创建函数类型，即新建 lambda 类型，并储存函数的形参以及对应类型和函数的返回值
  - 这种方式需要拓展原有的 `SyntaxTree::Type`
- 将函数与变量分开维护在两个 `unordered_map` 中
  - 这种方式在未来拓展判断函数与变量名字不能相同时可能会有些麻烦
- 附加一个 `unordered_map` 保存函数形参类型信息

第一种方式可能更优雅一点，但对于现在的实验目标来说有点 overdesign 了，因此最后选择了第三种方式

#### 其他困难

得益于 clangd 对 cmake 的支持，以及实验过程中尽可能使用新的 cpp 标准（不过由于 `CMakeLists` 指定的仅到 C++11，不太确定实验平台的环境，因此更新的一些标准的语法并没有使用），没有使用裸指针而全部使用智能指针等，本次实验过程中并没有遇到内存 bug，只有一些逻辑上的 bug，比较容易解决

### 难点与考察倾向

- 需要熟悉词法作用域（Lexical Scope）：
  - 进入新的 `BlockStmt` 时，创建新的词法作用域
  - 函数参数要保留一份在与函数体 `BlockStmt` 相同的词法作用域内，因此需要注意，如果函数创建了新的词法作用域，那么紧随之后的 `BlockStmt` 就不能再新建词法作用域，否则以下代码：
    ```c
    int main(int argc) {
      int argc = 1;
    }
    ```
    并不报错（因为参数与 `BlockStmt` 处在了两个作用域），但实际上这里有变量的重复定义
- 需要清楚递归遍历语法树的过程，不要遗漏了结点，并且注意符号保存到符号表的时机

### 实现思路与亮点

#### ctest

首先维护自动测试流程：

```cmake
set(executable sysYFParser)

# test executable
add_executable(integrationTestWrapper
  ${CMAKE_CURRENT_SOURCE_DIR}/test/test.cpp)

# enable testing functionality
enable_testing()

set(TESTCASES 2/01 2/02 2/03 2/04 2/05 2/06 2/07 2/11 2/12 2/13 2/14 2/15 2/16 2/17 3/01 3/02 3/03 3/04 3/05 3/06 3/07)
FOREACH(testcase ${TESTCASES})
  set(testcase_name IntegrationTest_${testcase})
  add_test(NAME ${testcase_name} COMMAND integrationTestWrapper ${executable} ../test/task${testcase}.sy)
  file(READ test/task${testcase}.out expected_output LIMIT_INPUT 1)
  set_tests_properties(${testcase_name} PROPERTIES PASS_REGULAR_EXPRESSION ${expected_output})
ENDFOREACH(testcase)
```

ctest 似乎并不支持获取测试具体 exit code（只能获取是否为 0），为了更精准的进行测试，这里包装了一个 `integrationTestWrapper` 可执行文件，将参数转发到 `sysYFParser` 并打印 exit code

在 `make` 之后就可以用 `ctest` 进行测试了，以下是最终测试结果：

```shell
......
      Start 20: IntegrationTest_3/06
20/21 Test #20: IntegrationTest_3/06 .............   Passed    0.00 sec
      Start 21: IntegrationTest_3/07
21/21 Test #21: IntegrationTest_3/07 .............   Passed    0.00 sec

100% tests passed, 0 tests failed out of 21

Total Test time (real) =   0.08 sec
```

#### Frame

本实验中创建了一个 `Frame` 类用来维护词法作用域

```cpp
class Frame {
    using VarType = std::unordered_map<std::string, SyntaxTree::Type>;
    using FuncParamsType =
        std::unordered_map<std::string, std::vector<SyntaxTree::Type>>;

  public:
    Frame() = default;
    explicit Frame(std::shared_ptr<Frame> parent) : parent(std::move(parent)) {}
    // define symbol type
    bool define(const std::string &symbol, SyntaxTree::Type type);
    // define function params. It's called after define function
    void define_params(const std::string &symbol,
                       std::vector<SyntaxTree::Type> &params);
    // lookup symbol type. The second indicates whether the symbol is found
    std::pair<SyntaxTree::Type, bool> lookup(std::string &symbol);
    // lookup function params. It's called after lookup function name symbol
    std::vector<SyntaxTree::Type> &lookup_params(std::string &symbol);

  private:
    std::shared_ptr<Frame> parent;
    VarType var_type;
    FuncParamsType func_params_type;
};
```

Frame 类会保存指向 parent Frame 的指针，lookup 操作从当前 Frame 开始，如果没有找到就一直向 parent 结点寻找，以实现不同作用域变量的隔离

#### 函数形参

```cpp
class SyntaxTreeChecker : public SyntaxTree::Visitor {
  public:
    ......
  private:
    ErrorReporter &err;
    SyntaxTree::Type expr_type;
    std::vector<SyntaxTree::Type> func_params_type;
    bool skip_next_create_new_frame;
    std::shared_ptr<Frame> frame;
};
```

增加了 `func_params_type` 用于维护形参类型数组

```cpp
void SyntaxTreeChecker::visit(FuncFParamList &node) {
    std::vector<SyntaxTree::Type> params_type;
    for (const auto &param : node.params) {
        params_type.push_back(param->param_type);
        param->accept(*this);
    }
    func_params_type = std::move(params_type);
}
```

之后，在遍历函数形参列表时会将形参的类型加入这一数组

```cpp
void SyntaxTreeChecker::visit(FuncDef &node) {
    // define function in global frame
    if (!frame->define(node.name, node.ret_type)) {
        err.error(node.loc, "Function definations are duplicated.");
        exit(static_cast<int>(ErrorType::FuncDuplicated));
    }

    // now we are going to enter the function, create a new frame
    const auto prev_frame = frame;
    frame = std::make_shared<Frame>(prev_frame);

    // visit params first, add them to cur_frame
    node.param_list->accept(*this);
    // save function params info in prev_frame (now it's the global frame)
    prev_frame->define_params(node.name, func_params_type);

    // we're already in the new frame
    skip_next_create_new_frame = true;
    // visit function body
    node.body->accept(*this);

    // exit the function frame
    frame = prev_frame;
}
```

在访问函数定义时，会将形参类型数组和函数类型（返回值）都保存到 Frame 中

#### 类型检查

为了匹配函数形参和实参的类型，需要能够得到每一个 `Expr` 的类型

这个拓展是比较简单的，原先的实现中已经做到了判断一个 `Expr` 是不是 `int`，我们只需要在原先的基础上，把它改为 `SyntaxTree::type expr_type` 变量，存储具体变量类型即可

例如：

```cpp
void SyntaxTreeChecker::visit(BinaryExpr &node) {
    node.lhs->accept(*this);
    const auto lhs_type = expr_type;
    node.rhs->accept(*this);
    const auto rhs_type = expr_type;
    if (node.op == SyntaxTree::BinOp::MODULO) {
        if (!(lhs_type == SyntaxTree::Type::INT) ||
            !(rhs_type == SyntaxTree::Type::INT)) {
            err.error(node.loc, "Operands of modulo should be integers.");
            exit(static_cast<int>(ErrorType::Modulo));
        }
    }
    if (lhs_type == SyntaxTree::Type::FLOAT ||
        rhs_type == SyntaxTree::Type::FLOAT)
        expr_type = SyntaxTree::Type::FLOAT;
    else
        expr_type = SyntaxTree::Type::INT;
}
```

对于二元运算，判断操作数是否含有 float 来决定 expr_type

在实现了具体判断 `Expr` 的类型后，取模运算的检查就非常平凡了，不再赘述

#### 其余 visit 函数的实现

- 注意不要遗漏子结点，例如 `IfStmt` 需要访问 `cond_exp`, `if_statement` 和 `else_statement`
- 注意如果这是个表达式，需要更改 visitor 的 `expr_type`

## 3-2

> 试回答：处理变量声明时，是应该先处理变量的初始化还是先把该变量加入符号表？

- 处理变量声明时，应该先处理变量的初始化，`int a[] = {a[0]};` 是不合法的
- 处理函数定义时，应该先把函数名加入符号表，因为需要支持递归调用

## 3-3

> 有人说如果本次实验允许修改抽象语法树的话，他想给 `Expr` 添加一个属性用于判断是否为整数，而不是在 `SyntaxTreeChecker` 内部设置该属性。这种做法有什么缺点？

将类型检查的一部分功能耦合到了语法分析部分，不利于代码模块化及维护

> 另一个人说，完成本次实验后 `SyntaxTreeChecker` 类过于庞大了，他想拆成多个类，每个类处理一种可能的语义错误，这种做法又有什么缺点？

需要多次遍历语法树，时间开销变大，而且结点间的访问遍历逻辑是相同的，且拆成多个类后，这一逻辑并不方便复用

## 3-4

> 如果想给错误人为地规定一个优先级，例如，如果取模运算是最高优先级，那么语义检查器在遇到存在取模运算错误的程序时，必定报取模运算错误，也即`ErrorType::Module`。对于如下程序：
>
> ```cpp
> int f(int x)
> {
>     return 0;
> }
> int main(){
>     int a = f(0.1);
>     return a % 0.1;
> }
> ```
>
> 将报错`ErrorType::Module`，试谈一下你的实现思路。

遇到错误并不立即退出，而是给出默认值，收集错误信息到一个容器中（例如 `std::vector`）并继续向下分析，在分析结束后再统一处理错误信息，报优先级最高的错误（并且其他的错误也可以随后输出）
