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
    Generator(NodeProg root, string filename) : root(root){
        gen_prog();
        ofstream output_file(filename);
        output_file << assembly;
        output_file.close();
    }

    string get_assembly() {
        return assembly;
    }
};