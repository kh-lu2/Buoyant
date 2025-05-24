#pragma once

#include "structs.hpp"
#include <unordered_map>

class Generator {
private:
    NodeProg root;
    string assembly;
    Stack stack;

    void gen_prog() {
        assembly = root.generate(stack);
    }

public:
    Generator(NodeProg root) : root(root){
        gen_prog();
    }

    string get_assembly() {
        return assembly;
    }
};