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

string tokens_to_asm(const vector<Token>& tokens) {
    string assembly = "global _start\n_start:\n";
    for (int i = 0; i < tokens.size(); i++) {
        const Token& token = tokens[i];
        if (token.type== TokenType::ret) {
            if (i + 2 < tokens.size() &&
                tokens[i + 1].type == TokenType::integer &&
                tokens[i + 2].type == TokenType::statement_end) {
                    assembly += "    mov rax, 60\n";
                    assembly += "    mov rdi, " + to_string(tokens[i + 1].value.value()) + "\n";
                    assembly += "    syscall\n";
                }
        }
    }
    return assembly;
}


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
    Generator generator(parser.get_node_exit());


    ofstream output_file("output_files/out.asm");
    output_file << generator.get_assembly();
    output_file.close();
    
    system("cd output_files && nasm -felf64 out.asm");
    system("cd output_files && ld -o out out.o");

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

*/