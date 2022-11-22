#ifndef _SYSYF_SYNTAX_TREE_CHECKER_H_
#define _SYSYF_SYNTAX_TREE_CHECKER_H_

#include "ErrorReporter.h"
#include "SyntaxTree.h"
#include <cassert>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

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

class SyntaxTreeChecker : public SyntaxTree::Visitor {
  public:
    explicit SyntaxTreeChecker(ErrorReporter &e) : err(e), frame(new Frame()) {}
    void visit(SyntaxTree::Assembly &node) override;
    void visit(SyntaxTree::FuncDef &node) override;
    void visit(SyntaxTree::BinaryExpr &node) override;
    void visit(SyntaxTree::UnaryExpr &node) override;
    void visit(SyntaxTree::LVal &node) override;
    void visit(SyntaxTree::Literal &node) override;
    void visit(SyntaxTree::ReturnStmt &node) override;
    void visit(SyntaxTree::VarDef &node) override;
    void visit(SyntaxTree::AssignStmt &node) override;
    void visit(SyntaxTree::FuncCallStmt &node) override;
    void visit(SyntaxTree::BlockStmt &node) override;
    void visit(SyntaxTree::EmptyStmt &node) override;
    void visit(SyntaxTree::ExprStmt &node) override;
    void visit(SyntaxTree::FuncParam &node) override;
    void visit(SyntaxTree::FuncFParamList &node) override;
    void visit(SyntaxTree::BinaryCondExpr &node) override;
    void visit(SyntaxTree::UnaryCondExpr &node) override;
    void visit(SyntaxTree::IfStmt &node) override;
    void visit(SyntaxTree::WhileStmt &node) override;
    void visit(SyntaxTree::BreakStmt &node) override;
    void visit(SyntaxTree::ContinueStmt &node) override;
    void visit(SyntaxTree::InitVal &node) override;

  private:
    ErrorReporter &err;
    SyntaxTree::Type expr_type;
    std::vector<SyntaxTree::Type> func_params_type;
    bool skip_next_create_new_frame;
    std::shared_ptr<Frame> frame;
};

enum class ErrorType {
    Accepted = 0,
    Modulo,
    VarUnknown,
    VarDuplicated,
    FuncUnknown,
    FuncDuplicated,
    FuncParams
};

#endif // _SYSYF_SYNTAX_TREE_CHECKER_H_