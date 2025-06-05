#include <optional>
#include <string>
#include <variant>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <stack>

#include "../inc/astnodes.h"
#include "../inc/stack.hpp"
#include "../inc/tokens.hpp"

using namespace std;

string NodeMathExpr::generate(Stack& S, string operation) const {
    string assembly;
    assembly += rhs->generate(S);
    assembly += lhs->generate(S);
    assembly += S.pop("rax");
    assembly += S.pop("rbx");
    assembly += operation;
    assembly += S.push("rax");
    return assembly;
};

string NodeMathExprAdd::generate(Stack& S) const {
    return NodeMathExpr::generate(S, "    add rax, rbx\n");
}

string NodeMathExprSub::generate(Stack& S) const {
    return NodeMathExpr::generate(S, "    sub rax, rbx\n");
}


string NodeMathExprMul::generate(Stack& S) const {
    return NodeMathExpr::generate(S, "    mul rbx\n");
}

string NodeMathExprDiv::generate(Stack& S) const {
    return NodeMathExpr::generate(S, "    div rbx\n");
}

string NodeTermNum::generate(Stack& S) const {
    string assembly;
    assembly += "    mov rax, " + token.value.value() + "\n";
    assembly += S.push("rax");
    return assembly;
}

string NodeTermVar::generate (Stack& S) const {
    if (!S.variables.contains(token.value.value())) {
        cerr << "No variable like that " + token.location() + "\n";
        exit(30);
    }
    return S.push("qword [rsp + " + to_string((S.stack_ptr - S.variables[token.value.value()] - 1) * 8) + "]");
}

string NodeStmtSmplRet::generate(Stack& S) const {
    string assembly;
    assembly += node_expr->generate(S);
    assembly += "    mov rax, 60\n";
    assembly += S.pop("rdi");
    assembly += "    syscall\n";
    return assembly;
}

string NodeStmtSmplVar::generate(Stack& S) const {
    string name = ident.value.value();
    if (S.variables.contains(name)) {
        string assembly;
        assembly += node_expr->generate(S);
        assembly += S.pop("rax");

        assembly += "    mov [rsp + " + to_string((S.stack_ptr - S.variables[name] - 1) * 8) + "], rax\n";
        return assembly;
    }
    S.variables[name] = S.stack_ptr;
    S.variable_stack.push(name);
    return node_expr->generate(S);
}

string NodeScope::generate(Stack& S) const {
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

string NodeIf::generate(Stack& S, string end_label) const {
    string assembly;
    assembly += node_expr->generate(S);
    assembly += S.pop("rax");
    string label = S.create_label();
    assembly += "    test rax, rax\n";
    assembly += "    jz " + label + "\n";
    assembly += scope->generate(S);
    assembly += "    jmp " + end_label + "\n";
    assembly += label + ":\n";
    if (after_if.has_value()) {
        assembly += after_if.value()->generate(S, end_label);
    }
    assembly += end_label + ":\n";
    return assembly;
}

string NodeStmtIf::generate(Stack& S) const {
    string end_label = S.create_label();
    return NodeIf::generate(S, end_label);
}

string NodeAfterIfElif::generate(Stack& S, string end_label) const {
    return NodeIf::generate(S, end_label);
}

string NodeAfterIfElse::generate(Stack& S, string end_label) const {
    return scope->generate(S);
}

string NodeProg::generate(Stack& S) const {
    string assembly;
    assembly += "global _start\n_start:\n";
    assembly += NodeScope::generate(S);
    assembly += "    mov rax, 60\n";
    assembly += "    mov rdi, 0\n";
    assembly += "    syscall\n";
    return assembly;
}