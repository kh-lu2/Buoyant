#pragma once
#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <optional>
#include <algorithm>

#include "structs.hpp"

using namespace std;
using filesystem::path;

const string var_characters = "@#$%";


class Lexer {
private:
    ifstream source_file;
    vector<Token> tokens;

    void tokenize() {
        string token;
        while (source_file >> token) {
            if (all_of(token.begin(), token.end(), ::isdigit)) {
                tokens.push_back({TokenType::integer, token});
            } else if (token == "&") {
                tokens.push_back({TokenType::ret});
            } else if (token == ".") {
                tokens.push_back({TokenType::stmt_begin_end});
            } else if (token == ",") {
                tokens.push_back({TokenType::stmt_middle});
            } else if (token == "~") {
                tokens.push_back({TokenType::assign_expr});
            } else if (token == "~~") {
                tokens.push_back({TokenType::assign_zero});
            } else if (token == "+") {
                tokens.push_back({TokenType::addition});
            } else if (token == "-") {
                tokens.push_back({TokenType::substraction});
            } else if (token == "o") {
                tokens.push_back({TokenType::multiplication});
            } else if (token == ":") {
                tokens.push_back({TokenType::division});
            } else if (token == "(") {
                tokens.push_back({TokenType::expr_start});
            } else if (token == ")") {
                tokens.push_back({TokenType::expr_end});
            } else if (token == "[") {
                tokens.push_back({TokenType::if_start});
            } else if (token == "]") {
                tokens.push_back({TokenType::if_end});
            } else if (token == "{") {
                tokens.push_back({TokenType::elif_start});
            } else if (token == "}") {
                tokens.push_back({TokenType::elif_end});
            } else if (token == "!") {
                tokens.push_back({TokenType::els});
            } else if (token == "^") {
                tokens.push_back({TokenType::scope_end});
            } else {
                for (auto &c: token) {
                    if (var_characters.find(c) == string::npos) {
                        cerr << "Buoya does not support that\n";
                        exit(10);
                    }
                }
                tokens.push_back({TokenType::identifier, token});
            }
        }
    }

public:
    Lexer(path filepath) : source_file(filepath) {
        if (source_file.fail()) {
            cerr << "Something wrong with file or filepath\n";
            exit(3);
        }
        tokenize();
    }

    vector<Token> get_tokens() {
        return tokens;
    }
};