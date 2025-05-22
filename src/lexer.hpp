#pragma once
#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <optional>

using namespace std;
using filesystem::path;

const string var_characters = "@#$%";

enum class TokenType {
    ret,
    integer,
    stmt_begin_end,
    stmt_middle,
    identifier,
    assign_zero,
    assign_expr
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
                } else if (token == ".") {
                    tokens.push_back({TokenType::stmt_begin_end});
                } else if (token == ",") {
                    tokens.push_back({TokenType::stmt_middle});
                } else if (token == "~") {
                    tokens.push_back({TokenType::assign_expr});
                } else if (token == "~~") {
                    tokens.push_back({TokenType::assign_zero});
                } else {
                    for (auto &c: token) {
                        if (var_characters.find(c) == string::npos) {
                            cerr << "Buoya does not support that\n";
                            exit(4);
                        }
                    }

                    tokens.push_back({TokenType::identifier, token});
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