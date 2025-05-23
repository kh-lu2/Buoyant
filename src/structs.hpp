#pragma once

#include <optional>
#include <string>
#include <variant>
#include <vector>
#include <iostream>
#include <unordered_map>

using namespace std;

enum class TokenType {
    ret,
    integer,
    stmt_begin_end,
    stmt_middle,
    identifier,
    assign_zero,
    assign_expr,
    add
};

struct Token {
    TokenType type;
    optional<string> value;
};

struct Var {
    int stack_index;
};

struct Stack {
    int stack_ptr = 0;
    unordered_map<string, Var> variables;

    string push(const string& reg) {
        stack_ptr++;
        return "    push " + reg + "\n";
    }

    string pop(const string& reg) {
        stack_ptr--;
        return "    pop " + reg + "\n";
    }    
};

struct NodeExpr {
    virtual string generate(Stack& S) const {}
    virtual ~NodeExpr() = default;
};

struct NodeBinExpr : NodeExpr {
    NodeExpr* lhs;
    NodeExpr* rhs;
    virtual string generate(Stack& S) const {}
    virtual ~NodeBinExpr() {
        delete lhs;
        delete rhs;
    }
};

struct NodeBinExprAdd : NodeBinExpr {
    string generate(Stack& S) const {
        string assembly;
        assembly += lhs->generate(S);
        assembly += rhs->generate(S);
        assembly += S.pop("rax");
        assembly += S.pop("rbx");
        assembly += "    add rax, rbx\n";
        assembly += S.push("rax");
        return assembly;
    }
};
struct NodeBinExprMul : NodeBinExpr {};

struct NodeTerm : NodeExpr {
    Token token;
    virtual string generate(Stack& S) const {}
    virtual ~NodeTerm() = default;
};

struct NodeTermInt : NodeTerm {
    Token token;
    string generate(Stack& S) const {
        string assembly;
        assembly += "    mov rax, " + token.value.value() + "\n";
        assembly += S.push("rax");
        return assembly;
    }
};

struct NodeTermIdent : NodeTerm {
    Token token;
    string generate (Stack& S) const {
        if (!S.variables.contains(token.value.value())) {
            cerr << "No variable like that\n";
            exit(10);
        }
        return S.push("qword [rsp + " + to_string((S.stack_ptr - S.variables[token.value.value()].stack_index - 1) * 8) + "]");
    }
};

struct NodeStmt {
    NodeExpr* node_expr;
    virtual string generate(Stack& S) const {}
 
    virtual ~NodeStmt() {
        delete node_expr;
    };
};

struct NodeStmtRet : NodeStmt {
    string generate(Stack& S) const {
        string assembly;
        assembly += node_expr->generate(S);
        assembly += "    mov rax, 60\n";
        assembly += S.pop("rdi");
        assembly += "    syscall\n";
        return assembly;
    }
};

struct NodeStmtVar : NodeStmt {
    Token ident;
    string generate(Stack& S) const {
        if (S.variables.contains(ident.value.value())) {
            cerr << "Double declaration\n";
            exit(9);
        }
        S.variables[ident.value.value()] = Var{S.stack_ptr};
        return node_expr->generate(S);
    }
};

struct NodeProg {
    vector<NodeStmt*> stmts;
};