#pragma once

#include "parser.hpp"

class Generator {
private:
    NodeExit root;
    string assembly;

    void generate() {

    }
public:
    Generator(NodeExit root) : root(root){
        assembly = "global _start\n_start:\n";
        int expr_value = root.expr.integer.value.value();
        assembly += "    mov rax, 60\n";
        assembly += "    mov rdi, " + to_string(expr_value) + "\n";
        assembly += "    syscall\n";
    }

    string get_assembly() {
        return assembly;
    }
};