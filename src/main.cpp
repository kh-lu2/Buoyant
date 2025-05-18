#include <iostream>
#include <fstream>
#include <filesystem>
#include <optional>
#include <vector>

using namespace std;
using filesystem::path;

enum class TokenType {
    ret,
    integer,
    statement_end
};

struct Token {
    TokenType type;
    optional<int> value;
};

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

    ifstream source(filepath);
    if (source.fail()) {
        cerr << "Something wrong with file or filepath...\n";
        return 3;
    }

    vector<Token> tokens;
    string str;
    while (source >> str) {
        try {
            int value = stoi(str);
            tokens.push_back({TokenType::integer, value});
        }
        catch (std::invalid_argument const& ex) {
            if (str == "&") {
                tokens.push_back({TokenType::ret});
            } else if (str == ">") {
                tokens.push_back({TokenType::statement_end});
            } else {
                cerr << "Buoya does not support that\n";
                return 4;
            }
        }
    }
    source.close();

    ofstream output_file("output_files/out.asm");
    output_file << tokens_to_asm(tokens);
    output_file.close();
    
    system("cd output_files && nasm -felf64 out.asm");
    system("cd output_files && ld -o out out.o");

    return 0;
}