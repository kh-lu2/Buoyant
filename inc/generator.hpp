#pragma once

#include <fstream>
#include "astnodes.h"

using namespace std;

class Generator {
private:
    NodeProg root;
    string assembly;
    Stack stack;

    void gen_prog() {
        assembly = root.generate(stack);
    }

public:
    Generator(const NodeProg& root, const string& filename) : root(root) {
        gen_prog();
        ofstream output_file(filename);
        output_file << assembly;
        output_file.close();
    }

    string get_assembly() const {
        return assembly;
    }
};