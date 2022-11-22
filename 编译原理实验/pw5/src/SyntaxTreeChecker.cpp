#include "SyntaxTreeChecker.h"

using SyntaxTree::Assembly;
using SyntaxTree::FuncDef;

using SyntaxTree::BinaryCondExpr;
using SyntaxTree::BinaryExpr;
using SyntaxTree::Literal;
using SyntaxTree::LVal;
using SyntaxTree::UnaryCondExpr;
using SyntaxTree::UnaryExpr;

using SyntaxTree::AssignStmt;
using SyntaxTree::BlockStmt;
using SyntaxTree::EmptyStmt;
using SyntaxTree::ExprStmt;
using SyntaxTree::FuncCallStmt;
using SyntaxTree::ReturnStmt;
using SyntaxTree::VarDef;

using SyntaxTree::FuncFParamList;
using SyntaxTree::FuncParam;

using SyntaxTree::BreakStmt;
using SyntaxTree::ContinueStmt;
using SyntaxTree::IfStmt;
using SyntaxTree::InitVal;
using SyntaxTree::WhileStmt;

bool Frame::define(const std::string &symbol, SyntaxTree::Type type) {
    return var_type.insert({symbol, type}).second;
}

void Frame::define_params(const std::string &symbol,
                          std::vector<SyntaxTree::Type> &params) {
    func_params_type.insert({symbol, params});
}

std::vector<SyntaxTree::Type> &Frame::lookup_params(std::string &symbol) {
    auto *cur_frame = this;
    while (cur_frame != nullptr) {
        if (cur_frame->func_params_type.count(symbol) != 0) {
            return cur_frame->func_params_type[symbol];
        }
        cur_frame = cur_frame->parent.get();
    }
    // should not reach here
    exit(static_cast<int>(ErrorType::FuncUnknown));
}

std::pair<SyntaxTree::Type, bool> Frame::lookup(std::string &symbol) {
    auto *cur_frame = this;
    while (cur_frame != nullptr) {
        if (cur_frame->var_type.count(symbol) != 0) {
            return {cur_frame->var_type[symbol], true};
        }
        cur_frame = cur_frame->parent.get();
    }
    // Since symbol not found, whatever the Type
    return {SyntaxTree::Type::VOID, false};
}

void SyntaxTreeChecker::visit(Assembly &node) {
    // Now we are in the global frame
    for (const auto &def : node.global_defs) {
        def->accept(*this);
    }
}

void SyntaxTreeChecker::visit(FuncDef &node) {
    // define function in global frame
    if (!frame->define(node.name, node.ret_type)) {
        err.error(node.loc, "Function already defined: " + node.name);
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

void SyntaxTreeChecker::visit(UnaryExpr &node) { node.rhs->accept(*this); }

void SyntaxTreeChecker::visit(LVal &node) {
    // For C++17, We hava pair decomposing, but it's C++11 (sad)
    const auto lookup_result = frame->lookup(node.name);
    const auto &type = lookup_result.first;
    const auto &success = lookup_result.second;
    if (!success) {
        err.error(node.loc, "Unknown variable: " + node.name);
        exit(static_cast<int>(ErrorType::VarUnknown));
    }
    expr_type = type;
}

void SyntaxTreeChecker::visit(Literal &node) { expr_type = node.literal_type; }

void SyntaxTreeChecker::visit(ReturnStmt &node) {
    if (node.ret) {
        node.ret->accept(*this);
    }
}

void SyntaxTreeChecker::visit(VarDef &node) {
    if (node.is_inited) {
        node.initializers->accept(*this);
    }
    if (!frame->define(node.name, node.btype)) {
        err.error(node.loc, "Duplicate definations of: " + node.name);
        exit(static_cast<int>(ErrorType::VarDuplicated));
    }
}

void SyntaxTreeChecker::visit(AssignStmt &node) {
    node.target->accept(*this);
    node.value->accept(*this);
}

void SyntaxTreeChecker::visit(FuncCallStmt &node) {
    // first lookup function name symbol
    const auto lookup_result = frame->lookup(node.name);
    const auto &type = lookup_result.first;
    const auto &success = lookup_result.second;
    if (!success) {
        err.error(node.loc, "Unknown function: " + node.name);
        exit(static_cast<int>(ErrorType::FuncUnknown));
    }

    // then collect argument types
    auto arguments_type = std::vector<SyntaxTree::Type>{};
    for (const auto &param : node.params) {
        param->accept(*this);
        arguments_type.push_back(expr_type);
    }

    // lookup param types and compare
    const auto params_type = frame->lookup_params(node.name);
    if (arguments_type != params_type) {
        err.error(node.loc, "Error function argument types.");
        exit(static_cast<int>(ErrorType::FuncParams));
    }

    // return value type
    expr_type = type;
}

void SyntaxTreeChecker::visit(BlockStmt &node) {
    // BlockStmt needs new Frame
    const auto prev_frame = frame;
    // But if the function have already created the frame, then skip
    if (skip_next_create_new_frame)
        skip_next_create_new_frame = false;
    else
        frame = std::make_shared<Frame>(prev_frame);

    // visit stmts
    for (const auto &stmt : node.body)
        stmt->accept(*this);

    frame = prev_frame;
}

void SyntaxTreeChecker::visit(EmptyStmt &node) {
    // empty
}

void SyntaxTreeChecker::visit(ExprStmt &node) { node.exp->accept(*this); }

void SyntaxTreeChecker::visit(FuncParam &node) {
    if (!frame->define(node.name, node.param_type)) {
        err.error(node.loc, "Duplicate definations of: " + node.name);
        exit(static_cast<int>(ErrorType::VarDuplicated));
    }
}

void SyntaxTreeChecker::visit(FuncFParamList &node) {
    std::vector<SyntaxTree::Type> params_type;
    for (const auto &param : node.params) {
        params_type.push_back(param->param_type);
        param->accept(*this);
    }
    func_params_type = std::move(params_type);
}

void SyntaxTreeChecker::visit(BinaryCondExpr &node) {
    node.lhs->accept(*this);
    node.rhs->accept(*this);
    // in sysYF, bool is int?
    expr_type = SyntaxTree::Type::INT;
}

void SyntaxTreeChecker::visit(UnaryCondExpr &node) {
    node.rhs->accept(*this);
    expr_type = SyntaxTree::Type::INT;
}

void SyntaxTreeChecker::visit(IfStmt &node) {
    node.cond_exp->accept(*this);
    node.if_statement->accept(*this);
    if (node.else_statement) {
        node.else_statement->accept(*this);
    }
}

void SyntaxTreeChecker::visit(WhileStmt &node) {
    node.cond_exp->accept(*this);
    node.statement->accept(*this);
}

void SyntaxTreeChecker::visit(BreakStmt &node) {
    // empty
}
void SyntaxTreeChecker::visit(ContinueStmt &node) {
    // empty
}

void SyntaxTreeChecker::visit(InitVal &node) {
    if (node.isExp) {
        node.expr->accept(*this);
    } else {
        for (const auto &element : node.elementList) {
            element->accept(*this);
        }
    }
}