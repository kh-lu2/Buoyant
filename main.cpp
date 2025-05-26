#include <vector>
#include <fstream>
#include <iostream>
#include <optional>
#include <filesystem>

#include "inc/lexer.h"
#include "inc/generator.hpp"
#include "inc/parser.h"

using namespace std;
using filesystem::path;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Wrong usage, should be: buoyant <source.bya>\n";
        return 1;
    }

    path filepath(argv[1]);
    if (filepath.extension() != ".bya") {
        cerr << "Wrong file extention, should be .bya\n";
        return 2;
    }

    Lexer lexer(filepath);
    Parser parser(lexer.get_tokens());
    Generator generator(parser.get_node_prog(), "build/out.asm");
    
    system("cd build && nasm -felf64 out.asm");
    system("cd build && ld -o out out.o");

    return 0;
}

/*
error codes:
0 -> everything ok
1 -> wrong usage
2 -> wrong file extention
3 -> ifstream failed

10 -> syntax buoya doesnt support (lexer)

20 -> Expected statement begin (parser)
21 -> Expected statement end (parser)
22 -> expected ..... end (parser)
23 -> expected scope end (parser)
24 -> Invalid term in an expression (parser)
25 -> invalid statement (parser)
26 -> invalid ....... statement (parser)

30 -> No variable like that (generator)
*/