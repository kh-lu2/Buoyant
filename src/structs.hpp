#pragma once

#include <optional>
#include <string>
#include <variant>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <stack>

using namespace std;

enum class TokenType {
    ret,
    integer,
    stmt_begin_end,
    stmt_middle,
    identifier,
    assign_zero,
    assign_expr,
    addition,
    substraction,
    multiplication,
    division,
    expr_start,
    expr_end,
    if_start,
    if_end,
    elif_start,
    elif_end,
    els,
    scope_end
};

struct Token {
    TokenType type;
    int line;
    int position;
    optional<string> value;

    string location(int offset = 0) const {
        return "at line " + to_string(line) + " and position " + to_string(position + offset);
    }
};

struct Var {
    int stack_index;
};

struct Stack {
    int stack_ptr = 0;
    int label_cnt = 0;
    unordered_map<string, Var> variables;
    stack<string> variable_stack;
    stack<int> scope_starts;

    string push(const string& reg) {
        stack_ptr++;
        return "    push " + reg + "\n";
    }

    string pop(const string& reg) {
        stack_ptr--;
        return "    pop " + reg + "\n";
    }    

    string create_label() {
        return "label" + to_string(label_cnt++);
    }
};

struct NodeExpr {
    virtual string generate(Stack& S) const {};
    virtual ~NodeExpr() = default;
};

struct NodeBinExpr : NodeExpr {
    NodeExpr* lhs;
    NodeExpr* rhs;
    virtual string generate(Stack& S, string operation) const {
        string assembly;
        assembly += rhs->generate(S);
        assembly += lhs->generate(S);
        assembly += S.pop("rax");
        assembly += S.pop("rbx");
        assembly += operation;
        assembly += S.push("rax");
        return assembly;
    };
    virtual ~NodeBinExpr() {
        delete lhs;
        delete rhs;
    }
};

struct NodeBinExprAdd : NodeBinExpr {
    string generate(Stack& S) const {
        return NodeBinExpr::generate(S, "    add rax, rbx\n");
    }
};

struct NodeBinExprSub : NodeBinExpr {
    string generate(Stack& S) const {
        return NodeBinExpr::generate(S, "    sub rax, rbx\n");
    }
};

struct NodeBinExprMul : NodeBinExpr {
    string generate(Stack& S) const {
        return NodeBinExpr::generate(S, "    mul rbx\n");
    }
};

struct NodeBinExprDiv : NodeBinExpr {
    string generate(Stack& S) const {
        return NodeBinExpr::generate(S, "    div rbx\n");
    }
};

struct NodeTerm : NodeExpr {
    Token token;
    virtual string generate(Stack& S) const {};
    virtual ~NodeTerm() = default;
};

struct NodeTermInt : NodeTerm {
    string generate(Stack& S) const {
        string assembly;
        assembly += "    mov rax, " + token.value.value() + "\n";
        assembly += S.push("rax");
        return assembly;
    }
};

struct NodeTermIdent : NodeTerm {
    string generate (Stack& S) const {
        if (!S.variables.contains(token.value.value())) {
            cerr << "No variable like that " + token.location() + "\n";
            exit(30);
        }
        return S.push("qword [rsp + " + to_string((S.stack_ptr - S.variables[token.value.value()].stack_index - 1) * 8) + "]");
    }
};


struct NodeStmt {
    virtual string generate(Stack& S) const {};
    virtual ~NodeStmt() = default;
};

struct NodeStmtExpr : NodeStmt {
    NodeExpr* node_expr;
    virtual string generate(Stack& S) const {};
 
    virtual ~NodeStmtExpr() {
        delete node_expr;
    };
};

struct NodeStmtExprRet : NodeStmtExpr {
    string generate(Stack& S) const {
        string assembly;
        assembly += node_expr->generate(S);
        assembly += "    mov rax, 60\n";
        assembly += S.pop("rdi");
        assembly += "    syscall\n";
        return assembly;
    }
};

struct NodeStmtExprVar : NodeStmtExpr {
    Token ident;
    string generate(Stack& S) const {
        string name = ident.value.value();
        if (S.variables.contains(name)) {
            string assembly;
            assembly += node_expr->generate(S);
            assembly += S.pop("rax");

            assembly += "    mov [rsp + " + to_string((S.stack_ptr - S.variables[name].stack_index - 1) * 8) + "], rax\n";
            return assembly;
        }
        S.variables[name] = Var{S.stack_ptr};
        S.variable_stack.push(name);
        return node_expr->generate(S);
    }
};

struct NodeScope : NodeStmt {
    vector<NodeStmt*> stmts;

    virtual string generate(Stack& S) const {
        string assembly;
        S.scope_starts.push(S.variables.size());

        for (auto &StmtPtr: stmts) {
            assembly += StmtPtr->generate(S) + "\n";
        }
        
        int pop_count = S.variable_stack.size() - S.scope_starts.top();
        S.scope_starts.pop();
        assembly += "    add rsp, " + to_string(pop_count * 8) + "\n";
        S.stack_ptr -= pop_count;

        while (pop_count--) {
            S.variables.erase(S.variable_stack.top());
            S.variable_stack.pop();
        }
        return assembly;
    }
};

struct NodeAfterIf {
    virtual string generate(Stack &S, string end_label) const {}
    virtual ~NodeAfterIf() = default;
};

struct NodeStmtIf : NodeStmt {
    NodeExpr* node_expr;
    NodeScope* scope;
    optional<NodeAfterIf*> after_if;
    string generate(Stack& S) const {
        string assembly;
        assembly += node_expr->generate(S);
        assembly += S.pop("rax");
        string label = S.create_label();
        assembly += "    test rax, rax\n";
        assembly += "    jz " + label + "\n";
        assembly += scope->generate(S);

        if (after_if.has_value()) {
            string end_label = S.create_label();
            assembly += "    jmp " + end_label + "\n";
            assembly += label + ":\n";
            assembly += after_if.value()->generate(S, end_label);
            assembly += end_label + ":\n";
        } else {
            assembly += label + ":\n";
        }
        return assembly;
    }
};

struct NodeAfterIfElif : NodeAfterIf {
    NodeExpr* node_expr;
    NodeScope* scope;
    optional<NodeAfterIf*> after_if; 

    string generate(Stack& S, string end_label) const {
        string assembly;
        assembly += node_expr->generate(S);
        assembly += S.pop("rax");
        string label = S.create_label();
        assembly += "    test rax, rax\n";
        assembly += "    jz " + label + "\n";
        assembly += scope->generate(S);
        assembly += "    jmp " + end_label + "\n";
        if (after_if.has_value()) {
            assembly += label + ":\n";
            assembly += after_if.value()->generate(S, end_label);
        } else {
            assembly += label + ":\n";
        }
        return assembly;
    }
};

struct NodeAfterIfElse : NodeAfterIf {
    NodeScope* scope;
    string generate(Stack& S, string end_label) const {
        string assembly;
        assembly += scope->generate(S);
        return assembly;
    }
};

struct NodeProg : NodeScope {
    string generate(Stack& S) const {
        string assembly;
        assembly += "global _start\n_start:\n";
        assembly += NodeScope::generate(S);
        assembly += "    mov rax, 60\n";
        assembly += "    mov rdi, 0\n";
        assembly += "    syscall\n";
        return assembly;
    }
};