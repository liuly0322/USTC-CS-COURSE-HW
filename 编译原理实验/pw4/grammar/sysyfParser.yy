%skeleton "lalr1.cc" /* -*- c++ -*- */
%require "3.0"
%defines
//%define parser_class_name {sysyfParser}
%define api.parser.class {sysyfParser}

%define api.token.constructor
%define api.value.type variant
%define parse.assert

%code requires
{
#include <string>
#include "SyntaxTree.h"
class sysyfDriver;
}

// The parsing context.
%param { sysyfDriver& driver }

// Location tracking
%locations
%initial-action
{
// Initialize the initial location.
@$.begin.filename = @$.end.filename = &driver.file;
};

// Enable tracing and verbose errors (which may be wrong!)
%define parse.trace
%define parse.error verbose

// Parser needs to know about the driver:
%code
{
#include "sysyfDriver.h"
#define yylex driver.lexer.yylex
}

// Tokens:
%define api.token.prefix {TOK_}

%token END
/*********add your token here*********/
/* 1. key words */
%token VOID INT FLOAT CONST
%token IF ELSE WHILE BREAK CONTINUE RETURN
/* 2. signs */
%token COMMA SEMICOLON
%token L_PAREN R_PAREN L_SQUARE_PAREN R_SQUARE_PAREN L_CURLY_PAREN R_CURLY_PAREN
%token ASSIGN EQUAL LESS LESS_EQUAL GREATER GREATER_EQUAL NOT_EQUAL
%token PLUS MINUS MULTIPLY DIVIDE MODULO
%token LOGIC_AND LOGIC_OR LOGIC_NOT
/* 3. others */
%token <int>INTCONST
%token <float>FLOATCONST
%token <std::string>IDENT


// Use variant-based semantic values: %type and %token expect genuine types
%type <SyntaxTree::Assembly*>CompUnit
%type <SyntaxTree::PtrList<SyntaxTree::GlobalDef>>GlobalDecl
/*********add semantic value definition here*********/
%type <SyntaxTree::PtrList<SyntaxTree::VarDef>>ConstDecl
%type <SyntaxTree::Type>BType
%type <SyntaxTree::PtrList<SyntaxTree::VarDef>>ConstDefs
%type <SyntaxTree::VarDef*>ConstDef
%type <SyntaxTree::PtrList<SyntaxTree::Expr>>Dimensions
%type <SyntaxTree::PtrList<SyntaxTree::InitVal>>ConstInitVals
%type <SyntaxTree::InitVal*>ConstInitVal
%type <SyntaxTree::PtrList<SyntaxTree::VarDef>>VarDecl
%type <SyntaxTree::PtrList<SyntaxTree::VarDef>>VarDefs
%type <SyntaxTree::VarDef*>VarDef
%type <SyntaxTree::InitVal*>InitVal
%type <SyntaxTree::PtrList<SyntaxTree::InitVal>>InitVals
%type <SyntaxTree::FuncDef*>FuncDef
%type <SyntaxTree::FuncFParamList*>FuncFParams
%type <SyntaxTree::FuncParam*>FuncFParam
%type <SyntaxTree::BlockStmt*>Block
%type <SyntaxTree::PtrList<SyntaxTree::Stmt>>BlockItems
%type <SyntaxTree::PtrList<SyntaxTree::Stmt>>BlockItem
%type <SyntaxTree::Stmt*>Stmt
%type <SyntaxTree::Stmt*>StmtMatched
%type <SyntaxTree::Stmt*>StmtUnMatched
%type <SyntaxTree::Expr*>Exp
%type <SyntaxTree::Expr*>Cond
%type <SyntaxTree::LVal*>LVal
%type <SyntaxTree::Expr*>PrimaryExp
%type <SyntaxTree::Literal*>Number
%type <SyntaxTree::Expr*>UnaryExp
%type <SyntaxTree::PtrList<SyntaxTree::Expr>>FuncRParams
%type <SyntaxTree::Expr*>MulExp
%type <SyntaxTree::Expr*>AddExp
%type <SyntaxTree::Expr*>RelExp
%type <SyntaxTree::Expr*>EqExp
%type <SyntaxTree::Expr*>LAndExp
%type <SyntaxTree::Expr*>LOrExp
%type <SyntaxTree::Expr*>ConstExp

// No %destructors are needed, since memory will be reclaimed by the
// regular destructors.

// Grammar:
%start Begin 

%%
Begin: CompUnit END {
    $1->loc = @$;
    driver.root = $1;
    return 0;
  }
  ;

CompUnit: CompUnit GlobalDecl {
		$1->global_defs.insert($1->global_defs.end(), $2.begin(), $2.end());
		$$ = $1;
	} 
	| GlobalDecl {
		$$ = new SyntaxTree::Assembly();
		$$->global_defs.insert($$->global_defs.end(), $1.begin(), $1.end());
  }
	;

GlobalDecl: ConstDecl {
    $$ = SyntaxTree::PtrList<SyntaxTree::GlobalDef>();
    $$.insert($$.end(), $1.begin(), $1.end());
  }
  | VarDecl {
    $$ = SyntaxTree::PtrList<SyntaxTree::GlobalDef>();
    $$.insert($$.end(), $1.begin(), $1.end());
  }
  | FuncDef {
    $$ = SyntaxTree::PtrList<SyntaxTree::GlobalDef>();
    $$.push_back(SyntaxTree::Ptr<SyntaxTree::GlobalDef>($1));
  }

ConstDecl: CONST BType ConstDefs SEMICOLON {
    $$ = $3;
    for (auto& node: $$) {
      node->btype = $2;
    }
  }

BType: VOID {
    $$ = SyntaxTree::Type::VOID;
  }
  |
  INT {
    $$ = SyntaxTree::Type::INT;
  }
  | FLOAT {
    $$ = SyntaxTree::Type::FLOAT;
  }

ConstDefs: ConstDef {
    $$ = SyntaxTree::PtrList<SyntaxTree::VarDef>();
    $$.push_back(SyntaxTree::Ptr<SyntaxTree::VarDef>($1));
  }
  | ConstDefs COMMA ConstDef {
    $1.push_back(SyntaxTree::Ptr<SyntaxTree::VarDef>($3));
    $$ = $1;
  }

ConstDef: IDENT Dimensions ASSIGN ConstInitVal {
    $$ = new SyntaxTree::VarDef();
    $$->is_constant = true;
    $$->name = $1;
    $$->is_inited = true;
    $$->array_length = $2;
    $$->initializers = SyntaxTree::Ptr<SyntaxTree::InitVal>($4);
    $$->loc = @$;
  }
  | IDENT ASSIGN ConstInitVal {
    $$ = new SyntaxTree::VarDef();
    $$->is_constant = true;
    $$->name = $1;
    $$->is_inited = true;
    $$->initializers = SyntaxTree::Ptr<SyntaxTree::InitVal>($3);
    $$->loc = @$;
  }

Dimensions: L_SQUARE_PAREN ConstExp R_SQUARE_PAREN {
    $$ = SyntaxTree::PtrList<SyntaxTree::Expr>();
    $$.push_back(SyntaxTree::Ptr<SyntaxTree::Expr>($2));
  }
  | Dimensions L_SQUARE_PAREN ConstExp R_SQUARE_PAREN {
    $1.push_back(SyntaxTree::Ptr<SyntaxTree::Expr>($3));
    $$ = $1;
  }

ConstInitVal: ConstExp {
    $$ = new SyntaxTree::InitVal();
    $$->isExp = true;
    $$->expr = SyntaxTree::Ptr<SyntaxTree::Expr>($1);
  }
  | L_CURLY_PAREN R_CURLY_PAREN {
    $$ = new SyntaxTree::InitVal();
    $$->isExp = false;
    $$->elementList = SyntaxTree::PtrList<SyntaxTree::InitVal>();
  }
  | L_CURLY_PAREN ConstInitVals R_CURLY_PAREN {
    $$ = new SyntaxTree::InitVal();
    $$->isExp = false;
    $$->elementList = $2;
  }

ConstInitVals: ConstInitVal {
    $$ = SyntaxTree::PtrList<SyntaxTree::InitVal>();
    $$.push_back(SyntaxTree::Ptr<SyntaxTree::InitVal>($1));
  }
  | ConstInitVals COMMA ConstInitVal {
    $1.push_back(SyntaxTree::Ptr<SyntaxTree::InitVal>($3));
    $$ = $1;
  }

VarDecl: BType VarDefs SEMICOLON {
    for (auto& vardef: $2) {
      vardef->btype = $1;
    }
    $$ = $2;
  } 

VarDefs: VarDef {
    $$ = SyntaxTree::PtrList<SyntaxTree::VarDef>();
    $$.push_back(SyntaxTree::Ptr<SyntaxTree::VarDef>($1));
  }
  | VarDefs COMMA VarDef {
    $1.push_back(SyntaxTree::Ptr<SyntaxTree::VarDef>($3));
    $$ = $1;
  }

VarDef: IDENT {
    $$ = new SyntaxTree::VarDef();
    $$->is_constant = false;
    $$->name = $1;
    $$->is_inited = false;
    $$->loc = @$;
  }
  | IDENT ASSIGN InitVal {
    $$ = new SyntaxTree::VarDef();
    $$->is_constant = false;
    $$->name = $1;
    $$->is_inited = true;
    $$->initializers = SyntaxTree::Ptr<SyntaxTree::InitVal>($3);
    $$->loc = @$;
  }
  | IDENT Dimensions {
    $$ = new SyntaxTree::VarDef();
    $$->is_constant = false;
    $$->name = $1;
    $$->is_inited = false;
    $$->array_length = $2;
    $$->loc = @$;
  }
  | IDENT Dimensions ASSIGN InitVal {
    $$ = new SyntaxTree::VarDef();
    $$->is_constant = false;
    $$->name = $1;
    $$->is_inited = true;
    $$->array_length = $2;
    $$->initializers = SyntaxTree::Ptr<SyntaxTree::InitVal>($4);
    $$->loc = @$;
  }

InitVal: Exp {
    $$ = new SyntaxTree::InitVal();
    $$->isExp = true;
    $$->expr = SyntaxTree::Ptr<SyntaxTree::Expr>($1);
    $$->elementList = SyntaxTree::PtrList<SyntaxTree::InitVal>();
    $$->loc = @$;
  }
  | L_CURLY_PAREN R_CURLY_PAREN {
    $$ = new SyntaxTree::InitVal();
    $$->isExp = false;
    $$->elementList = SyntaxTree::PtrList<SyntaxTree::InitVal>();
    $$->loc = @$;
  }
  | L_CURLY_PAREN InitVals R_CURLY_PAREN {
    $$ = new SyntaxTree::InitVal();
    $$->isExp = false;
    $$->elementList = $2;
    $$->loc = @$;
  }

InitVals: InitVal {
    $$ = SyntaxTree::PtrList<SyntaxTree::InitVal>();
    $$.push_back(SyntaxTree::Ptr<SyntaxTree::InitVal>($1));
  }
  | InitVals COMMA InitVal {
    $1.push_back(SyntaxTree::Ptr<SyntaxTree::InitVal>($3));
    $$ = $1;
  }

FuncDef: BType IDENT L_PAREN R_PAREN Block {
    $$ = new SyntaxTree::FuncDef();
    $$->ret_type = $1;
    auto empty_args = new SyntaxTree::FuncFParamList();
    $$->param_list = SyntaxTree::Ptr<SyntaxTree::FuncFParamList>(empty_args);
    $$->name = $2;
    $$->body = SyntaxTree::Ptr<SyntaxTree::BlockStmt>($5);
  }
  | BType IDENT L_PAREN FuncFParams R_PAREN Block {
    $$ = new SyntaxTree::FuncDef();
    $$->ret_type = $1;
    $$->param_list = SyntaxTree::Ptr<SyntaxTree::FuncFParamList>($4);
    $$->name = $2;
    $$->body = SyntaxTree::Ptr<SyntaxTree::BlockStmt>($6);
  }

FuncFParams: FuncFParam {
    $$ = new SyntaxTree::FuncFParamList();
    $$->params.push_back(SyntaxTree::Ptr<SyntaxTree::FuncParam>($1));
  }
  | FuncFParams COMMA FuncFParam {
    $1->params.push_back(SyntaxTree::Ptr<SyntaxTree::FuncParam>($3));
    $$ = $1;
  }

FuncFParam: BType IDENT {
    $$ = new SyntaxTree::FuncParam();
    $$->name = $2;
    $$->param_type = $1;
    $$->array_index = SyntaxTree::PtrList<SyntaxTree::Expr>();
  }
  | BType IDENT Dimensions {
    $$ = new SyntaxTree::FuncParam();
    $$->name = $2;
    $$->param_type = $1;
    $$->array_index = $3;
  }
  | BType IDENT L_SQUARE_PAREN R_SQUARE_PAREN {
    $$ = new SyntaxTree::FuncParam();
    $$->name = $2;
    $$->param_type = $1;
    $$->array_index = SyntaxTree::PtrList<SyntaxTree::Expr>();
    $$->array_index.insert($$->array_index.begin(),NULL);
  }
  | BType IDENT L_SQUARE_PAREN R_SQUARE_PAREN Dimensions {
    $$ = new SyntaxTree::FuncParam();
    $$->name = $2;
    $$->param_type = $1;
    $$->array_index = $5;
    $$->array_index.insert($$->array_index.begin(),NULL);
    $$->loc = @$;
  }
  
Block: L_CURLY_PAREN R_CURLY_PAREN {
    $$ = new SyntaxTree::BlockStmt();
    $$->body = SyntaxTree::PtrList<SyntaxTree::Stmt>();
  }
  | L_CURLY_PAREN BlockItems R_CURLY_PAREN {
    $$ = new SyntaxTree::BlockStmt();
    $$->body = $2;
  }

BlockItems: BlockItem {
    $$ = $1;
  }
  | BlockItems BlockItem {
    $1.insert($1.end(), $2.begin(), $2.end());
    $$ = $1;
  }

BlockItem: ConstDecl {
    $$ = SyntaxTree::PtrList<SyntaxTree::Stmt>();
    $$.insert($$.end(), $1.begin(), $1.end());
  }
  | VarDecl {
    $$ = SyntaxTree::PtrList<SyntaxTree::Stmt>();
    $$.insert($$.end(), $1.begin(), $1.end());
  }
  | Stmt {
    $$ = SyntaxTree::PtrList<SyntaxTree::Stmt>();
    $$.push_back(SyntaxTree::Ptr<SyntaxTree::Stmt>($1));
  }

Stmt: StmtMatched {
    $$ = $1;
  }
  | StmtUnMatched {
    $$ = $1;
  }

StmtMatched: LVal ASSIGN Exp SEMICOLON {
    auto assign_stmt = new SyntaxTree::AssignStmt();
    assign_stmt->target = SyntaxTree::Ptr<SyntaxTree::LVal>($1);
    assign_stmt->value = SyntaxTree::Ptr<SyntaxTree::Expr>($3);
    $$ = assign_stmt;
    $$->loc = @$;
  }
  | SEMICOLON {
    $$ = new SyntaxTree::EmptyStmt();
    $$->loc = @$;
  }
  | Exp SEMICOLON {
    auto expr_stmt = new SyntaxTree::ExprStmt();
    expr_stmt->exp = SyntaxTree::Ptr<SyntaxTree::Expr>($1);
    $$ = expr_stmt;
    $$->loc = @$;
  }
  | Block {
    $$ = $1;
  }
  | IF L_PAREN Cond R_PAREN StmtMatched ELSE StmtMatched {
    auto if_stmt = new SyntaxTree::IfStmt();
    if_stmt->cond_exp = SyntaxTree::Ptr<SyntaxTree::Expr>($3);
    if_stmt->if_statement = SyntaxTree::Ptr<SyntaxTree::Stmt>($5);
    if_stmt->else_statement = SyntaxTree::Ptr<SyntaxTree::Stmt>($7);
    $$ = if_stmt;
    $$->loc = @$;
  }
  | WHILE L_PAREN Cond R_PAREN StmtMatched {
    auto while_stmt = new SyntaxTree::WhileStmt();
    while_stmt->cond_exp = SyntaxTree::Ptr<SyntaxTree::Expr>($3);
    while_stmt->statement = SyntaxTree::Ptr<SyntaxTree::Stmt>($5);
    $$ = while_stmt;
    $$->loc = @$;
  }
  | BREAK SEMICOLON {
    $$ = new SyntaxTree::BreakStmt();
  }
  | CONTINUE SEMICOLON {
    $$ = new SyntaxTree::ContinueStmt();
  }
  | RETURN SEMICOLON {
    auto ret_stmt = new SyntaxTree::ReturnStmt();
    ret_stmt->ret = SyntaxTree::Ptr<SyntaxTree::Expr>(nullptr);
    $$ = ret_stmt;
    $$->loc = @$;
  }
  | RETURN Exp SEMICOLON {
    auto ret_stmt = new SyntaxTree::ReturnStmt();
    ret_stmt->ret = SyntaxTree::Ptr<SyntaxTree::Expr>($2);
    $$ = ret_stmt;
    $$->loc = @$;
  }

StmtUnMatched: IF L_PAREN Cond R_PAREN Stmt {
    auto if_stmt = new SyntaxTree::IfStmt();
    if_stmt->cond_exp = SyntaxTree::Ptr<SyntaxTree::Expr>($3);
    if_stmt->if_statement = SyntaxTree::Ptr<SyntaxTree::Stmt>($5);
    $$ = if_stmt;
    $$->loc = @$;
  }
  | IF L_PAREN Cond R_PAREN StmtMatched ELSE StmtUnMatched {
    auto if_stmt = new SyntaxTree::IfStmt();
    if_stmt->cond_exp = SyntaxTree::Ptr<SyntaxTree::Expr>($3);
    if_stmt->if_statement = SyntaxTree::Ptr<SyntaxTree::Stmt>($5);
    if_stmt->else_statement = SyntaxTree::Ptr<SyntaxTree::Stmt>($7);
    $$ = if_stmt;
    $$->loc = @$;
  }
  | WHILE L_PAREN Cond R_PAREN StmtUnMatched {
    auto while_stmt = new SyntaxTree::WhileStmt();
    while_stmt->cond_exp = SyntaxTree::Ptr<SyntaxTree::Expr>($3);
    while_stmt->statement = SyntaxTree::Ptr<SyntaxTree::Stmt>($5);
    $$ = while_stmt;
    $$->loc = @$;
  }

Exp: AddExp {
    $$ = $1;
  }

Cond: LOrExp {
    $$ = $1;
  }

LVal: IDENT {
    $$ = new SyntaxTree::LVal();
    $$->name = $1;
    $$->array_index = SyntaxTree::PtrList<SyntaxTree::Expr>();
    $$->loc = @$;
  }
  | IDENT Dimensions {
    $$ = new SyntaxTree::LVal();
    $$->name = $1;
    $$->array_index = $2;
    $$->loc = @$;
  }

PrimaryExp: LVal {
    $$ = $1;
  }
  | Number {
    $$ = $1;
  }
  | L_PAREN Exp R_PAREN {
    $$ = $2;
  }

Number: INTCONST {
    $$ = new SyntaxTree::Literal();
    $$->literal_type = SyntaxTree::Type::INT;
    $$->int_const = $1;
    $$->loc = @$;
  }
  | FLOATCONST {
    $$ = new SyntaxTree::Literal();
    $$->literal_type = SyntaxTree::Type::FLOAT;
    $$->float_const = $1;
    $$->loc = @$;
  }

UnaryExp: PrimaryExp {
    $$ = $1;
  }
  | PLUS UnaryExp {
    auto expr = new SyntaxTree::UnaryExpr();
    expr->op = SyntaxTree::UnaryOp::PLUS;
    expr->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>($2);
    $$ = expr;
    $$->loc = @$;
  }
  | MINUS UnaryExp {
    auto expr = new SyntaxTree::UnaryExpr();
    expr->op = SyntaxTree::UnaryOp::MINUS;
    expr->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>($2);
    $$ = expr;
    $$->loc = @$;
  }
  | LOGIC_NOT UnaryExp {
    auto expr = new SyntaxTree::UnaryCondExpr();
    expr->op = SyntaxTree::UnaryCondOp::NOT;
    expr->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>($2);
    $$ = expr;
    $$->loc = @$;
  }
  | IDENT L_PAREN R_PAREN {
    auto expr = new SyntaxTree::FuncCallStmt();
    expr->name = $1;
    expr->params = SyntaxTree::PtrList<SyntaxTree::Expr>();
    $$ = expr;
    $$->loc = @$;
  }
  | IDENT L_PAREN FuncRParams R_PAREN {
    auto expr = new SyntaxTree::FuncCallStmt();
    expr->name = $1;
    expr->params = $3;
    $$ = expr;
    $$->loc = @$;
  }

FuncRParams: Exp {
    $$ = SyntaxTree::PtrList<SyntaxTree::Expr>();
    $$.push_back(SyntaxTree::Ptr<SyntaxTree::Expr>($1));
  }
  | FuncRParams COMMA Exp {
    $1.push_back(SyntaxTree::Ptr<SyntaxTree::Expr>($3));
    $$ = $1;
  }

MulExp: UnaryExp {
    $$ = $1;
  }
  | MulExp MULTIPLY UnaryExp {
    auto expr = new SyntaxTree::BinaryExpr();
    expr->op = SyntaxTree::BinOp::MULTIPLY;
    expr->lhs = SyntaxTree::Ptr<SyntaxTree::Expr>($1);
    expr->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>($3);
    $$ = expr;
    $$->loc = @$;
  }
  | MulExp DIVIDE UnaryExp {
    auto expr = new SyntaxTree::BinaryExpr();
    expr->op = SyntaxTree::BinOp::DIVIDE;
    expr->lhs = SyntaxTree::Ptr<SyntaxTree::Expr>($1);
    expr->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>($3);
    $$ = expr;
    $$->loc = @$;
  }
  | MulExp MODULO UnaryExp {
    auto expr = new SyntaxTree::BinaryExpr();
    expr->op = SyntaxTree::BinOp::MODULO;
    expr->lhs = SyntaxTree::Ptr<SyntaxTree::Expr>($1);
    expr->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>($3);  
    $$ = expr;
    $$->loc = @$;
  }

AddExp: MulExp {
    $$ = $1;
  }
  | AddExp PLUS MulExp {
    auto expr = new SyntaxTree::BinaryExpr();
    expr->op = SyntaxTree::BinOp::PLUS;
    expr->lhs = SyntaxTree::Ptr<SyntaxTree::Expr>($1);
    expr->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>($3);
    $$ = expr;
    $$->loc = @$;
  }
  | AddExp MINUS MulExp {
    auto expr = new SyntaxTree::BinaryExpr();
    expr->op = SyntaxTree::BinOp::MINUS;
    expr->lhs = SyntaxTree::Ptr<SyntaxTree::Expr>($1);
    expr->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>($3);
    $$ = expr;
    $$->loc = @$;
  }

RelExp: AddExp {
    $$ = $1;
  }
  | RelExp LESS AddExp {
    auto expr = new SyntaxTree::BinaryCondExpr();
    expr->op = SyntaxTree::BinaryCondOp::LT;
    expr->lhs = SyntaxTree::Ptr<SyntaxTree::Expr>($1);
    expr->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>($3);
    $$ = expr;
    $$->loc = @$;
  }
  | RelExp LESS_EQUAL AddExp {
    auto expr = new SyntaxTree::BinaryCondExpr();
    expr->op = SyntaxTree::BinaryCondOp::LTE;
    expr->lhs = SyntaxTree::Ptr<SyntaxTree::Expr>($1);
    expr->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>($3);   
    $$ = expr;
    $$->loc = @$;
  }
  | RelExp GREATER AddExp {
    auto expr = new SyntaxTree::BinaryCondExpr();
    expr->op = SyntaxTree::BinaryCondOp::GT;
    expr->lhs = SyntaxTree::Ptr<SyntaxTree::Expr>($1);
    expr->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>($3);
    $$ = expr;
    $$->loc = @$;
  }
  | RelExp GREATER_EQUAL AddExp {
    auto expr = new SyntaxTree::BinaryCondExpr();
    expr->op = SyntaxTree::BinaryCondOp::GTE;
    expr->lhs = SyntaxTree::Ptr<SyntaxTree::Expr>($1);
    expr->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>($3);   
    $$ = expr;
    $$->loc = @$;
  }

EqExp: RelExp {
    $$ = $1;
  }
  | EqExp EQUAL RelExp {
    auto expr = new SyntaxTree::BinaryCondExpr();
    expr->op = SyntaxTree::BinaryCondOp::EQ;
    expr->lhs = SyntaxTree::Ptr<SyntaxTree::Expr>($1);
    expr->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>($3);
    $$ = expr;
    $$->loc = @$;
  }
  | EqExp NOT_EQUAL RelExp {
    auto expr = new SyntaxTree::BinaryCondExpr();
    expr->op = SyntaxTree::BinaryCondOp::NEQ;
    expr->lhs = SyntaxTree::Ptr<SyntaxTree::Expr>($1);
    expr->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>($3);
    $$ = expr;
    $$->loc = @$;
  }

LAndExp: EqExp {
    $$ = $1;
  }
  | LAndExp LOGIC_AND EqExp {
    auto expr = new SyntaxTree::BinaryCondExpr();
    expr->op = SyntaxTree::BinaryCondOp::LAND;
    expr->lhs = SyntaxTree::Ptr<SyntaxTree::Expr>($1);
    expr->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>($3);    
    $$ = expr;
    $$->loc = @$;
  }

LOrExp: LAndExp {
    $$ = $1;
  }
  | LOrExp LOGIC_OR LAndExp {
    auto expr = new SyntaxTree::BinaryCondExpr();
    expr->op = SyntaxTree::BinaryCondOp::LOR;
    expr->lhs = SyntaxTree::Ptr<SyntaxTree::Expr>($1);
    expr->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>($3);     
    $$ = expr;
    $$->loc = @$;
  }

ConstExp: AddExp {
    $$ = $1;
  }

/*********add other semantic symbol definition here*********/

%%

// Register errors to the driver:
void yy::sysyfParser::error (const location_type& l,
                          const std::string& m)
{
    driver.error(l, m);
}
