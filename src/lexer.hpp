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
    string source_file;
    vector<Token> tokens;
    int current = -1;

    optional<char> try_next() {
        if (current + 1 >= source_file.size()) return {};
        return source_file[current + 1];
    }

    bool next(char c) {
        return try_next().has_value() && try_next().value() == c;
    }

    optional<TokenType> cast_single(char c) {
        switch (c) {
            case '&':
                return TokenType::ret;
            case '.':
                return TokenType::stmt_begin_end;
            case ',':
                return TokenType::stmt_middle;
            case '+':
                return TokenType::addition;
            case '-':
                return TokenType::substraction;
            case 'o':
                return TokenType::multiplication;
            case ':':
                return TokenType::division;
            case '(':
                return TokenType::expr_start;
            case ')':
                return TokenType::expr_end;
            case '[':
                return TokenType::if_start;
            case ']':
                return TokenType::if_end;
            case '{':
                return TokenType::elif_start;
            case '}':
                return TokenType::elif_end;
            case '!':
                return TokenType::els;
            case '^':
                return TokenType::scope_end;
            default:
                return {};
        }
    }

    void tokenize() {
        while (try_next().has_value()) {
            char c = try_next().value();
            if (isspace(c)) {
                current++;
                continue;
            }
            optional<TokenType> token_type = cast_single(c);
            if (token_type.has_value()) {
                tokens.push_back({token_type.value()});
                current++;
            } else if (c == '~') {
                current++;
                if (next('~')) {
                    tokens.push_back({TokenType::assign_zero});
                    current++;
                } else {
                    tokens.push_back({TokenType::assign_expr});
                }
            } else if (c == '0') {
                tokens.push_back({TokenType::integer, "0"});
                current++;
            } else if (isdigit(c)) {
                string number {c};
                current++;
                optional<char> c = try_next();
                while (c.has_value() && isdigit(c.value())) {
                    number += {c.value()};
                    current++;
                    c = try_next();
                }
                tokens.push_back({TokenType::integer, number});                
            } else if (var_characters.find(c) != string::npos) {
                string var_name {c};
                current++;
                optional<char> c = try_next();
                while (c.has_value() && var_characters.find(c.value()) != string::npos) {
                    var_name += {c.value()};
                    current++;
                    c = try_next();
                }
                tokens.push_back({TokenType::identifier, var_name});
            } else {
                cerr << "Buoya does not support that\n";
                exit(10);
            }
        }
    }

public:
    Lexer(path filepath) : source_file(filepath) {
        ifstream source_stream(filepath);
        if (source_stream.fail()) {
            cerr << "Something wrong with file or filepath\n";
            exit(3);
        }
        stringstream buffer;
        buffer << source_stream.rdbuf();
        source_file = buffer.str();
        tokenize();
    }

    vector<Token> get_tokens() {
        return tokens;
    }
};