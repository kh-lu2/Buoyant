#pragma once

#include "structs.hpp"
#include <unordered_map>

class Generator {
private:
    NodeProg root;
    string assembly;
    Stack stack;

    void gen_prog() {
        assembly = "global _start\n_start:\n";

        for (auto &NodeStmtPtr: root.stmts) {
            assembly += NodeStmtPtr->generate(stack) + "\n";
        }

        assembly += "    mov rax, 60\n";
        assembly += "    mov rdi, 0\n";
        assembly += "    syscall\n";
    }

public:
    Generator(NodeProg root) : root(root){
        gen_prog();
    }

    string get_assembly() {
        return assembly;
    }
};