#pragma once
#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <optional>

using namespace std;
using filesystem::path;


enum class TokenType {
    ret,
    integer,
    stmt_begin,
    stmt_end,
    ident
};

struct Token {
    TokenType type;
    optional<string> value;
};


class Lexer {
private:
    ifstream source_file;
    vector<Token> tokens;

    void tokenize() {
        string token;
        while (source_file >> token) {
            try {
                int value = stoi(token);
                tokens.push_back({TokenType::integer, token});
            }
            catch (std::invalid_argument const& ex) {
                if (token == "&") {
                    tokens.push_back({TokenType::ret});
                } else if (token == "<") {
                    tokens.push_back({TokenType::stmt_begin});
                } else if (token == ">") {
                    tokens.push_back({TokenType::stmt_end});
                } else if (token == "@") {
                    tokens.push_back({TokenType::ident, token});
                } else {
                    cerr << "Buoya does not support that\n";
                    exit(4);
                }
            }
        }
    }

public:
    Lexer(path filepath) : source_file(filepath) {
        if (source_file.fail()) {
            cerr << "Something wrong with file or filepath...\n";
            exit(3);
        }
        tokenize();
    }

    vector<Token> get_tokens() {
        return tokens;
    }
};