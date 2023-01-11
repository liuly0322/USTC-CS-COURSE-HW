/**
 * @file IRBuilder.cpp
 * @author liuly (me@liuly.moe)
 * @brief IRBuilder 简易实现
 * @version 0.1
 * @date 2022-12-08
 * 一个 SysYF IRBuilder 的简易实现
 * - 修改框架，增加了 ErrorReporter
 * - 目前仅支持一维数组且未考虑考虑局部数组隐式初始化
 * @copyright Copyright (c) 2022
 */

#include "IRBuilder.h"
#include "BasicBlock.h"
#include "Function.h"
#include "Instruction.h"
#include "SyntaxTree.h"
#include <vector>

#define CONST_INT(num) ConstantInt::get(num, module.get())
#define CONST_FLOAT(num) ConstantFloat::get(num, module.get())

// Global variables and functions here to store state
// last visited expr
Value *prev_expr = nullptr;

// 函数栈内维护 While 嵌套结构
struct WhileStructType {
    BasicBlock *condBB;
    BasicBlock *innerBB;
    BasicBlock *exitBB;
};
auto curWhileStruct = WhileStructType{nullptr, nullptr, nullptr};

// 函数栈内维护跳转信息
struct CondStructType {
    BasicBlock *trueBB;
    BasicBlock *falseBB;
};
auto curCondStruct = CondStructType{nullptr, nullptr};

// types
Type *VOID_T;
Type *INT1_T;
Type *INT32_T;
Type *FLOAT_T;

Type *baseTypetoLLVMTy(SyntaxTree::Type type) {
    switch (type) {
    case SyntaxTree::Type::INT:
        return INT32_T;
    case SyntaxTree::Type::VOID:
        return VOID_T;
    case SyntaxTree::Type::BOOL:
        return INT1_T;
    case SyntaxTree::Type::FLOAT:
        return FLOAT_T;
    default:
        return nullptr;
    }
}

Type *SyntaxTreeTytoLLVMTy(SyntaxTree::Type type, bool isPtr = false) {
    Type *base_type = baseTypetoLLVMTy(type);
    if (base_type == nullptr || !isPtr) {
        return base_type;
    };
    return Type::get_pointer_type(base_type);
}

void IRBuilder::typeConvert(Type *expected) {
    // Constants
    if (dynamic_cast<Constant *>(prev_expr) != nullptr) {
        auto *prev_int = dynamic_cast<ConstantInt *>(prev_expr);
        auto *prev_float = dynamic_cast<ConstantFloat *>(prev_expr);
        if (prev_int != nullptr) {
            if (expected == FLOAT_T)
                prev_expr = CONST_FLOAT(prev_int->get_value());
            else if (expected == INT1_T)
                prev_expr = CONST_INT(prev_int->get_value() != 0);
        }
        if (prev_float != nullptr) {
            if (expected == INT32_T)
                prev_expr =
                    CONST_INT(static_cast<int>(prev_float->get_value()));
            else if (expected == INT1_T)
                prev_expr = CONST_INT(prev_float->get_value() != 0);
        }
        return;
    }

    auto *type = prev_expr->get_type();
    if (type == expected) {
        return;
    }

    // pointer deref
    if (type->is_pointer_type()) {
        type = type->get_pointer_element_type();
        prev_expr = builder->create_load(prev_expr);
    }

    if (type == INT32_T && expected == FLOAT_T) {
        prev_expr = builder->create_sitofp(prev_expr, expected);
        return;
    }
    if (type == FLOAT_T && expected == INT32_T) {
        prev_expr = builder->create_fptosi(prev_expr, expected);
        return;
    }
    if (type == INT32_T && expected == INT1_T) {
        prev_expr = builder->create_icmp_ne(prev_expr, CONST_INT(0));
        return;
    }
    if (type == FLOAT_T && expected == INT1_T) {
        prev_expr = builder->create_fcmp_ne(prev_expr, CONST_FLOAT(0));
        return;
    }
}

void IRBuilder::binOpGen(Value *lhs, Value *rhs, BinOp op) {
    // For Both Constants
    if ((dynamic_cast<Constant *>(lhs) != nullptr) &&
        (dynamic_cast<Constant *>(rhs) != nullptr)) {
        if ((dynamic_cast<ConstantFloat *>(lhs) != nullptr) ||
            (dynamic_cast<ConstantFloat *>(rhs) != nullptr)) {
            float lOperand;
            float rOperand;
            if (dynamic_cast<ConstantInt *>(lhs) != nullptr) {
                lOperand = dynamic_cast<ConstantInt *>(lhs)->get_value();
            } else {
                lOperand = dynamic_cast<ConstantFloat *>(lhs)->get_value();
            }
            if (dynamic_cast<ConstantInt *>(rhs) != nullptr) {
                rOperand = dynamic_cast<ConstantInt *>(rhs)->get_value();
            } else {
                rOperand = dynamic_cast<ConstantFloat *>(rhs)->get_value();
            }
            if (op.flag == BinOp::isBinOp) {
                switch (op.bin_op) {
                case SyntaxTree::BinOp::PLUS:
                    prev_expr = CONST_FLOAT(lOperand + rOperand);
                    break;
                case SyntaxTree::BinOp::MINUS:
                    prev_expr = CONST_FLOAT(lOperand - rOperand);
                    break;
                case SyntaxTree::BinOp::MULTIPLY:
                    prev_expr = CONST_FLOAT(lOperand * rOperand);
                    break;
                case SyntaxTree::BinOp::DIVIDE:
                    prev_expr = CONST_FLOAT(lOperand / rOperand);
                    break;
                default:
                    break;
                }
            } else {
                switch (op.bin_cond_op) {
                case SyntaxTree::BinaryCondOp::LT:
                    prev_expr = CONST_INT(lOperand < rOperand);
                    break;
                case SyntaxTree::BinaryCondOp::LTE:
                    prev_expr = CONST_INT(lOperand <= rOperand);
                    break;
                case SyntaxTree::BinaryCondOp::GT:
                    prev_expr = CONST_INT(lOperand > rOperand);
                    break;
                case SyntaxTree::BinaryCondOp::GTE:
                    prev_expr = CONST_INT(lOperand >= rOperand);
                    break;
                case SyntaxTree::BinaryCondOp::EQ:
                    prev_expr = CONST_INT(lOperand == rOperand);
                    break;
                case SyntaxTree::BinaryCondOp::NEQ:
                    prev_expr = CONST_INT(lOperand != rOperand);
                    break;
                default:
                    break;
                }
            }
        } else {
            const int lOperand = dynamic_cast<ConstantInt *>(lhs)->get_value();
            const int rOperand = dynamic_cast<ConstantInt *>(rhs)->get_value();
            if (op.flag == BinOp::isBinOp) {
                switch (op.bin_op) {
                case SyntaxTree::BinOp::PLUS:
                    prev_expr = CONST_INT(lOperand + rOperand);
                    break;
                case SyntaxTree::BinOp::MINUS:
                    prev_expr = CONST_INT(lOperand - rOperand);
                    break;
                case SyntaxTree::BinOp::MULTIPLY:
                    prev_expr = CONST_INT(lOperand * rOperand);
                    break;
                case SyntaxTree::BinOp::DIVIDE:
                    prev_expr = CONST_INT(lOperand / rOperand);
                    break;
                case SyntaxTree::BinOp::MODULO:
                    prev_expr = CONST_INT(lOperand % rOperand);
                    break;
                }
            } else {
                switch (op.bin_cond_op) {
                case SyntaxTree::BinaryCondOp::LT:
                    prev_expr = CONST_INT(lOperand < rOperand);
                    break;
                case SyntaxTree::BinaryCondOp::LTE:
                    prev_expr = CONST_INT(lOperand <= rOperand);
                    break;
                case SyntaxTree::BinaryCondOp::GT:
                    prev_expr = CONST_INT(lOperand > rOperand);
                    break;
                case SyntaxTree::BinaryCondOp::GTE:
                    prev_expr = CONST_INT(lOperand >= rOperand);
                    break;
                case SyntaxTree::BinaryCondOp::EQ:
                    prev_expr = CONST_INT(lOperand == rOperand);
                    break;
                case SyntaxTree::BinaryCondOp::NEQ:
                    prev_expr = CONST_INT(lOperand != rOperand);
                    break;
                default:
                    break;
                }
            }
        }
        return;
    }

    // For pointers
    if (lhs->get_type()->is_pointer_type()) {
        lhs = builder->create_load(lhs);
    }
    if (rhs->get_type()->is_pointer_type()) {
        rhs = builder->create_load(rhs);
    }

    // Type convert
    if (lhs->get_type()->is_float_type() &&
        rhs->get_type()->is_integer_type()) {
        rhs = builder->create_sitofp(rhs, FLOAT_T);
    }
    if (lhs->get_type()->is_integer_type() &&
        rhs->get_type()->is_float_type()) {
        lhs = builder->create_sitofp(lhs, FLOAT_T);
    }
    const bool is_int = lhs->get_type()->is_integer_type();
    if (op.flag == BinOp::isBinOp) {
        switch (op.bin_op) {
        case SyntaxTree::BinOp::PLUS:
            prev_expr = is_int ? builder->create_iadd(lhs, rhs)
                               : builder->create_fadd(lhs, rhs);
            break;
        case SyntaxTree::BinOp::MINUS:
            prev_expr = is_int ? builder->create_isub(lhs, rhs)
                               : builder->create_fsub(lhs, rhs);
            break;
        case SyntaxTree::BinOp::MULTIPLY:
            prev_expr = is_int ? builder->create_imul(lhs, rhs)
                               : builder->create_fmul(lhs, rhs);
            break;
        case SyntaxTree::BinOp::DIVIDE:
            prev_expr = is_int ? builder->create_isdiv(lhs, rhs)
                               : builder->create_fdiv(lhs, rhs);
            break;
        case SyntaxTree::BinOp::MODULO:
            prev_expr = builder->create_isrem(lhs, rhs);
            break;
        }
    } else {
        switch (op.bin_cond_op) {
        case SyntaxTree::BinaryCondOp::LT:
            if (is_int)
                prev_expr = builder->create_icmp_lt(lhs, rhs);
            else
                prev_expr = builder->create_fcmp_lt(lhs, rhs);
            break;
        case SyntaxTree::BinaryCondOp::LTE:
            if (is_int)
                prev_expr = builder->create_icmp_le(lhs, rhs);
            else
                prev_expr = builder->create_fcmp_le(lhs, rhs);
            break;
        case SyntaxTree::BinaryCondOp::GT:
            if (is_int)
                prev_expr = builder->create_icmp_gt(lhs, rhs);
            else
                prev_expr = builder->create_fcmp_gt(lhs, rhs);
            break;
        case SyntaxTree::BinaryCondOp::GTE:
            if (is_int)
                prev_expr = builder->create_icmp_ge(lhs, rhs);
            else
                prev_expr = builder->create_fcmp_ge(lhs, rhs);
            break;
        case SyntaxTree::BinaryCondOp::EQ:
            if (is_int)
                prev_expr = builder->create_icmp_eq(lhs, rhs);
            else
                prev_expr = builder->create_fcmp_eq(lhs, rhs);
            break;
        case SyntaxTree::BinaryCondOp::NEQ:
            if (is_int)
                prev_expr = builder->create_icmp_ne(lhs, rhs);
            else
                prev_expr = builder->create_fcmp_ne(lhs, rhs);
            break;
        default:
            break;
        }
    }
}

void IRBuilder::visit(SyntaxTree::Assembly &node) {
    VOID_T = Type::get_void_type(module.get());
    INT1_T = Type::get_int1_type(module.get());
    INT32_T = Type::get_int32_type(module.get());
    FLOAT_T = Type::get_float_type(module.get());
    for (const auto &def : node.global_defs) {
        def->accept(*this);
    }
}

void IRBuilder::visit(SyntaxTree::FuncDef &node) {
    // 访问函数声明，存储到全局作用域
    auto *ret_type = SyntaxTreeTytoLLVMTy(node.ret_type);
    std::vector<Type *> params{};
    for (const auto &param : node.param_list->params) {
        params.push_back(SyntaxTreeTytoLLVMTy(param->param_type,
                                              !param->array_index.empty()));
    }
    auto *FunTy = FunctionType::get(ret_type, params);
    auto *Fun = Function::create(FunTy, node.name, module.get());
    scope.push(node.name, Fun);

    // 处理函数体
    scope.enter();

    // 参数存储到函数内部作用域
    // 要分配栈上空间，使得拥有左值
    auto *entryBlock =
        BasicBlock::create(module.get(), node.name + "_entry", Fun);
    builder->set_insert_point(entryBlock);
    auto arg = Fun->arg_begin();
    auto arg_end = Fun->arg_end();
    auto param = node.param_list->params.begin();
    auto param_end = node.param_list->params.end();
    while (arg != arg_end && param != param_end) {
        auto name = (*param)->name;
        auto *val = *arg;
        auto *argAlloca = builder->create_alloca(val->get_type());
        builder->create_store(val, argAlloca);
        scope.push(name, argAlloca);
        arg++, param++;
    }

    // 访问函数体，翻译
    for (const auto &stmt : node.body->body) {
        stmt->accept(*this);
    }
    // 可能省略了返回语句
    if (builder->get_insert_block()->get_terminator() == nullptr) {
        if (ret_type == VOID_T) {
            builder->create_void_ret();
        } else if (ret_type == INT32_T) {
            builder->create_ret(CONST_INT(0));
        } else if (ret_type == FLOAT_T) {
            builder->create_ret(CONST_FLOAT(0));
        } else {
            err.error(node.loc, "Function with invalid return type!");
        }
    }

    scope.exit();
}

void IRBuilder::visit(SyntaxTree::VarDef &node) {
    // 计算类型（变量 or 数组）
    auto *element_type = baseTypetoLLVMTy(node.btype);
    auto *type = element_type;
    auto is_array = !node.array_length.empty();
    if (is_array) {
        node.array_length[0]->accept(*this);
        auto *const_int = dynamic_cast<ConstantInt *>(prev_expr);
        type = Type::get_array_type(type, const_int->get_value());
    }

    // 申请空间 或 常量
    Value *alloca;
    if (scope.in_global()) {
        if (!node.is_inited) {
            // 0 初始化
            auto *zero_initializer = ConstantZero::get(type, module.get());
            alloca = GlobalVariable::create(node.name, module.get(), type,
                                            false, zero_initializer);
        } else if (is_array) {
            // 已初始化的全局数组，是否是 const 由 create 函数控制
            std::vector<Constant *> initializer_values;
            for (const auto &val : node.initializers->elementList) {
                val->accept(*this);
                typeConvert(element_type);
                initializer_values.push_back(
                    dynamic_cast<Constant *>(prev_expr));
            }
            // 接下来 11 行是一个 patch。多维数组的时候需要优化掉
            auto array_length =
                static_cast<ArrayType *>(type)->get_num_of_elements();
            for (auto i = 0;
                 i < static_cast<int>(array_length) -
                         static_cast<int>(initializer_values.size());
                 i++) {
                prev_expr = CONST_INT(0);
                typeConvert(element_type);
                initializer_values.push_back(
                    dynamic_cast<Constant *>(prev_expr));
            }
            auto *initializer = ConstantArray::get(
                static_cast<ArrayType *>(type), initializer_values);
            alloca = GlobalVariable::create(node.name, module.get(), type,
                                            node.is_constant, initializer);
        } else {
            if (node.is_constant) {
                // 全局常量。可以被计算出来的（直接替换）
                node.initializers->expr->accept(*this);
                typeConvert(element_type);
                alloca = prev_expr;
            } else {
                // 正常分配
                node.initializers->expr->accept(*this);
                typeConvert(element_type);
                auto *const_expr = dynamic_cast<Constant *>(prev_expr);
                alloca = GlobalVariable::create(node.name, module.get(), type,
                                                false, const_expr);
            }
        }
    } else {
        // 统一在最开始的基本块分配变量，防止循环造成的内存泄漏
        auto *entryBB =
            builder->get_insert_block()->get_parent()->get_entry_block();
        // 事实上创建了一条新指令，所以需要先移除，再添加到开头
        alloca = AllocaInst::create_alloca(type, entryBB);
        entryBB->get_instructions().pop_back();
        entryBB->add_instr_begin(static_cast<Instruction *>(alloca));
    }

    // 保存到符号表
    scope.push(node.name, alloca);

    // 处理局部变量初始化
    if (scope.in_global() || !node.is_inited) {
        return;
    }
    if (!is_array) {
        node.initializers->expr->accept(*this);
        typeConvert(element_type);
        builder->create_store(prev_expr, alloca);
        return;
    }
    for (auto i = 0;
         i < static_cast<int>(node.initializers->elementList.size()); i++) {
        node.initializers->elementList[i]->accept(*this);
        typeConvert(element_type);
        auto *ptr = builder->create_gep(alloca, {CONST_INT(0), CONST_INT(i)});
        builder->create_store(prev_expr, ptr);
    }
}

void IRBuilder::visit(SyntaxTree::InitVal &node) { node.expr->accept(*this); }

void IRBuilder::visit(SyntaxTree::LVal &node) {
    // 变量，实际不一定具有左值（const 常量）
    // const 常量返回值，否则返回地址
    // 首先获取变量（地址）
    auto name = node.name;
    auto *ptr = scope.find(name, false);

    // 对变量名的访问
    if (node.array_index.empty()) {
        if (ptr->get_type()->is_pointer_type() &&
            ptr->get_type()->get_pointer_element_type()->is_array_type()) {
            // 如果是数组，需要解下第一维，转换成指针
            prev_expr = builder->create_gep(ptr, {CONST_INT(0), CONST_INT(0)});
        } else {
            prev_expr = ptr;
        }
        return;
    }

    // 对数组的访问，应该都有左值 ptr
    if (!ptr->get_type()->is_pointer_type()) {
        err.error(node.loc, "Variable should have a pointer type.");
    }

    std::vector<Value *> indexes{};
    bool index_all_const = true;
    // int a[2][3] 相比 int a[][3]，a 多了一个维度，需要补 0
    if (ptr->get_type()->get_pointer_element_type()->is_array_type()) {
        indexes.push_back(CONST_INT(0));
    } else {
        ptr = builder->create_load(ptr);
    }
    for (const auto &index : node.array_index) {
        index->accept(*this);
        auto *const_index = dynamic_cast<ConstantInt *>(prev_expr);
        if (const_index == nullptr) {
            index_all_const = false;
        }
        typeConvert(INT32_T);
        indexes.push_back(prev_expr);
    }

    // 如果数组是 const，下标也是 const，直接返回 Constant*
    auto *ptr_global = dynamic_cast<GlobalVariable *>(ptr);
    if (ptr_global != nullptr && ptr_global->is_const() && index_all_const) {
        auto *initArray = dynamic_cast<ConstantArray *>(ptr_global->get_init());
        for (auto &iter = ++indexes.begin(); iter != indexes.end(); iter++) {
            prev_expr = initArray->get_element_value(
                dynamic_cast<ConstantInt *>(*iter)->get_value());
        }
        return;
    }

    // 否则生成 gep
    prev_expr = builder->create_gep(ptr, indexes);
}

void IRBuilder::visit(SyntaxTree::AssignStmt &node) {
    node.target->accept(*this);
    auto *ptr = prev_expr;
    node.value->accept(*this);
    typeConvert(ptr->get_type()->get_pointer_element_type());
    auto *value = prev_expr;
    builder->create_store(value, ptr);
}

void IRBuilder::visit(SyntaxTree::Literal &node) {
    if (node.literal_type == SyntaxTree::Type::FLOAT) {
        prev_expr = CONST_FLOAT(node.float_const);
    } else if (node.literal_type == SyntaxTree::Type::INT) {
        prev_expr = CONST_INT(node.int_const);
    } else {
        err.error(node.loc, "Error literal type.");
    }
}

void IRBuilder::visit(SyntaxTree::ReturnStmt &node) {
    if (node.ret) {
        node.ret->accept(*this);
        auto *expected_type =
            builder->get_insert_block()->get_parent()->get_return_type();
        typeConvert(expected_type);
        builder->create_ret(prev_expr);
    } else {
        builder->create_void_ret();
    }
}

void IRBuilder::visit(SyntaxTree::BlockStmt &node) {
    scope.enter();
    for (const auto &stmt : node.body) {
        stmt->accept(*this);
    }
    scope.exit();
}

void IRBuilder::visit(SyntaxTree::ExprStmt &node) { node.exp->accept(*this); }

void IRBuilder::visit(SyntaxTree::UnaryCondExpr &node) {
    // 文法上，这个后面只能跟一元算数表达式，无法跟随条件表达式
    // 也就是 !(a == 0) 其实是不合法的。。。
    // 所以这里可以看成是：“计算”，最后输出还是 32 位 INT
    node.rhs->accept(*this);
    typeConvert(INT1_T);
    auto *const_int = dynamic_cast<ConstantInt *>(prev_expr);
    if (const_int != nullptr) {
        prev_expr = CONST_INT(const_int->get_value() == 0 ? 1 : 0);
        return;
    }
    prev_expr = builder->create_zext(prev_expr, INT32_T);
    prev_expr = builder->create_icmp_eq(prev_expr, CONST_INT(0));
    prev_expr = builder->create_zext(prev_expr, INT32_T);
}

void IRBuilder::visit(SyntaxTree::BinaryExpr &node) {
    node.lhs->accept(*this);
    auto *lhs = prev_expr;
    node.rhs->accept(*this);
    auto *rhs = prev_expr;
    binOpGen(lhs, rhs, BinOp(node.op));
}

void IRBuilder::visit(SyntaxTree::UnaryExpr &node) {
    node.rhs->accept(*this);
    if (node.op == SyntaxTree::UnaryOp::PLUS) {
        return;
    }
    auto *const_int = dynamic_cast<ConstantInt *>(prev_expr);
    auto *const_float = dynamic_cast<ConstantFloat *>(prev_expr);
    if (const_int != nullptr) {
        prev_expr = CONST_INT(-const_int->get_value());
    } else if (const_float != nullptr) {
        prev_expr = CONST_FLOAT(-const_float->get_value());
    } else {
        binOpGen(CONST_INT(0), prev_expr, BinOp(SyntaxTree::BinOp::MINUS));
    }
}

void IRBuilder::visit(SyntaxTree::FuncCallStmt &node) {
    auto name = node.name;
    std::vector<Value *> arguments{};
    auto param = node.params.begin();
    auto param_end = node.params.end();
    auto *Fun = dynamic_cast<Function *>(scope.find(name, true));
    auto arg = Fun->arg_begin();
    auto arg_end = Fun->arg_end();
    while (arg != arg_end && param != param_end) {
        (*param)->accept(*this);
        typeConvert((*arg)->get_type());
        arguments.push_back(prev_expr);
        arg++, param++;
    }
    prev_expr = builder->create_call(Fun, arguments);
}

void IRBuilder::visit(SyntaxTree::BinaryCondExpr &node) {
    auto op = node.op;
    auto *curFunction = builder->get_insert_block()->get_parent();
    if (op == SyntaxTree::BinaryCondOp::LAND ||
        op == SyntaxTree::BinaryCondOp::LOR) {
        auto *rhsBB = BasicBlock::create(module.get(), "", curFunction);

        auto prevCond = curCondStruct;
        curCondStruct = op == SyntaxTree::BinaryCondOp::LAND
                            ? CondStructType{rhsBB, prevCond.falseBB}
                            : CondStructType{prevCond.trueBB, rhsBB};
        node.lhs->accept(*this);
        if (dynamic_cast<SyntaxTree::BinaryCondExpr *>(node.lhs.get()) ==
            nullptr) {
            // 补上跳转
            typeConvert(INT1_T);
            builder->create_cond_br(prev_expr, curCondStruct.trueBB,
                                    curCondStruct.falseBB);
        }
        curCondStruct = prevCond;

        builder->set_insert_point(rhsBB);
        node.rhs->accept(*this);
        if (dynamic_cast<SyntaxTree::BinaryCondExpr *>(node.rhs.get()) ==
            nullptr) {
            // 补上跳转
            typeConvert(INT1_T);
            builder->create_cond_br(prev_expr, curCondStruct.trueBB,
                                    curCondStruct.falseBB);
        }
        return;
    }
    node.lhs->accept(*this);
    auto *lhs = prev_expr;
    node.rhs->accept(*this);
    auto *rhs = prev_expr;
    binOpGen(lhs, rhs, BinOp(node.op));
    builder->create_cond_br(prev_expr, curCondStruct.trueBB,
                            curCondStruct.falseBB);
}

void IRBuilder::visit(SyntaxTree::IfStmt &node) {
    const auto prev_cond_struct = curCondStruct;

    auto *curFunction = builder->get_insert_block()->get_parent();
    auto *trueBB = BasicBlock::create(module.get(), "", curFunction);
    auto *falseBB = BasicBlock::create(module.get(), "", curFunction);
    auto *exitBB = node.else_statement
                       ? BasicBlock::create(module.get(), "", curFunction)
                       : falseBB;

    curCondStruct = CondStructType{trueBB, falseBB};
    // 访问条件。可能是逻辑表达式（跳转），也可能需要转换
    node.cond_exp->accept(*this);
    if (builder->get_insert_block()->get_terminator() == nullptr) {
        typeConvert(INT1_T);
        builder->create_cond_br(prev_expr, trueBB, falseBB);
    }
    curCondStruct = prev_cond_struct;

    builder->set_insert_point(trueBB);
    node.if_statement->accept(*this);
    if (builder->get_insert_block()->get_terminator() == nullptr)
        builder->create_br(exitBB);

    if (node.else_statement) {
        builder->set_insert_point(falseBB);
        node.else_statement->accept(*this);
        if (builder->get_insert_block()->get_terminator() == nullptr)
            builder->create_br(exitBB);
    }

    builder->set_insert_point(exitBB);
}

void IRBuilder::visit(SyntaxTree::WhileStmt &node) {
    auto *curFunction = builder->get_insert_block()->get_parent();
    const auto prev_while_struct = curWhileStruct;

    // 新创建的 BB，更新 WhileStruct
    auto *condBB = BasicBlock::create(module.get(), "", curFunction);
    auto *innerBB = BasicBlock::create(module.get(), "", curFunction);
    auto *exitBB = BasicBlock::create(module.get(), "", curFunction);

    curWhileStruct = WhileStructType{condBB, innerBB, exitBB};

    // 条件。需要维护条件信息并还原
    if (builder->get_insert_block()->get_terminator() == nullptr) {
        builder->create_br(condBB);
    }
    builder->set_insert_point(condBB);
    const auto prev_cond_struct = curCondStruct;
    curCondStruct = CondStructType{innerBB, exitBB};
    // 访问条件。可能是逻辑表达式（跳转），也可能需要转换
    node.cond_exp->accept(*this);
    if (builder->get_insert_block()->get_terminator() == nullptr) {
        typeConvert(INT1_T);
        builder->create_cond_br(prev_expr, innerBB, exitBB);
    }
    curCondStruct = prev_cond_struct;

    // while 内部
    builder->set_insert_point(innerBB);
    node.statement->accept(*this);
    if (builder->get_insert_block()->get_terminator() == nullptr)
        builder->create_br(condBB);

    // 出口基本块
    builder->set_insert_point(exitBB);

    curWhileStruct = prev_while_struct;
}

void IRBuilder::visit(SyntaxTree::BreakStmt & /*node*/) {
    builder->create_br(curWhileStruct.exitBB);
}

void IRBuilder::visit(SyntaxTree::ContinueStmt & /*node*/) {
    builder->create_br(curWhileStruct.condBB);
}

// empty
void IRBuilder::visit(SyntaxTree::FuncFParamList &node) {}
void IRBuilder::visit(SyntaxTree::FuncParam &node) {}
void IRBuilder::visit(SyntaxTree::EmptyStmt &node) {}
