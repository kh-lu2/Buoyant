#include <vector>
#include <fstream>
#include <iostream>
#include <optional>
#include <filesystem>

#include "lexer.hpp"
#include "parser.hpp"
#include "generator.hpp"

using namespace std;
using filesystem::path;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Wrong usage. Should be: buoyant <source.bya>\n";
        return 1;
    }

    path filepath(argv[1]);
    if (filepath.extension() != ".bya") {
        cerr << "Wrong file extention! Should be .bya\n";
        return 2;
    }

    Lexer lexer(filepath);
    Parser parser(lexer.get_tokens());
    Generator generator(parser.get_node_prog());


    ofstream output_file("build/out.asm");
    output_file << generator.get_assembly();
    output_file.close();
    
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
4 -> syntax buoya doesnt support (lexer)
5 -> invalid expression (parser)
6 -> no statement end
7 -> no statement after statement begin
8 -> syntax error
*/