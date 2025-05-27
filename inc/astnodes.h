#pragma once

#include <optional>
#include <vector>
#include <iostream>
#include <unordered_map>
#include "../inc/stack.hpp"
#include "../inc/tokens.hpp"

using namespace std;

struct NodeExpr {
    virtual string generate(Stack& S) const = 0;
    virtual ~NodeExpr() = default;
};

struct NodeMathExpr : NodeExpr {
    NodeExpr* lhs;
    NodeExpr* rhs;
    virtual string generate(Stack& S, string operation) const;
    virtual ~NodeMathExpr();
};

struct NodeMathExprAdd : NodeMathExpr {
    string generate(Stack& S) const override;
};

struct NodeMathExprSub : NodeMathExpr {
    string generate(Stack& S) const override;
};

struct NodeMathExprMul : NodeMathExpr {
    string generate(Stack& S) const override;
};

struct NodeMathExprDiv : NodeMathExpr {
    string generate(Stack& S) const override;
};

struct NodeTerm : NodeExpr {
    Token token;
    virtual string generate(Stack& S) const = 0;
    virtual ~NodeTerm() = default;
};

struct NodeTermNum : NodeTerm {
    string generate(Stack& S) const override;
};

struct NodeTermVar : NodeTerm {
    string generate (Stack& S) const override;
};

struct NodeStmt {
    virtual string generate(Stack& S) const = 0;
    virtual ~NodeStmt() = default;
};

struct NodeStmtSmpl : NodeStmt {
    NodeExpr* node_expr;
    virtual string generate(Stack& S) const = 0;
    virtual ~NodeStmtSmpl();
};

struct NodeStmtSmplRet : NodeStmtSmpl {
    string generate(Stack& S) const override;
};

struct NodeStmtSmplVar : NodeStmtSmpl {
    Token ident;
    string generate(Stack& S) const override;
};

struct NodeScope : NodeStmt {
    vector<NodeStmt*> stmts;
    virtual string generate(Stack& S) const override;
};

struct NodeAfterIf {
    virtual string generate(Stack &S, string end_label) const = 0;
    virtual ~NodeAfterIf() = default;
};

struct NodeIf {
    NodeExpr* node_expr;
    NodeScope* scope;
    optional<NodeAfterIf*> after_if;
    virtual string generate(Stack& S, string end_label) const;
};

struct NodeStmtIf : NodeStmt, NodeIf {
    string generate(Stack& S) const override;
};

struct NodeAfterIfElif : NodeAfterIf, NodeIf {
    string generate(Stack& S, string end_label) const override;
};

struct NodeAfterIfElse : NodeAfterIf {
    NodeScope* scope;
    string generate(Stack& S, string end_label) const override;
};

struct NodeProg : NodeScope {
    string generate(Stack& S) const override;
};