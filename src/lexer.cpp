#include <fstream>
#include "../inc/lexer.h"

using namespace std;
using filesystem::path;

optional<char> Lexer::try_next() const {
    if (current + 1 >= source_file.size()) return {};
    return source_file[current + 1];
}

bool Lexer::next(const char& c) const {
    return try_next().has_value() && try_next().value() == c;
}

void Lexer::go_forward() {
    current++;
    curr_pos++;
}

optional<TokenType> Lexer::cast_single(const char& c) const {
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

void Lexer::tokenize() {
    while (try_next().has_value()) {
        char c = try_next().value();
        if (isspace(c)) {
            go_forward();
            if (c == '\n') {
                curr_line++;
                curr_pos = 0;
            }
            continue;
        }
        optional<TokenType> token_type = cast_single(c);
        if (token_type.has_value()) {
            go_forward();
            tokens.push_back({token_type.value(), curr_line, curr_pos});
        } else if (c == '~') {
            go_forward();
            if (next('~')) {
                go_forward();
                tokens.push_back({TokenType::assign_zero, curr_line, curr_pos});
            } else
                tokens.push_back({TokenType::assign_expr, curr_line, curr_pos});
        } else if (c == '0') {
            go_forward();
            tokens.push_back({TokenType::number, curr_line, curr_pos, "0"});
        } else if (isdigit(c)) {
            go_forward();
            string number {c};
            int num_pos = curr_pos;
            optional<char> next_char = try_next();
            while (next_char.has_value() && isdigit(next_char.value())) {
                number += {next_char.value()};
                go_forward();
                next_char = try_next();
            }
            tokens.push_back({TokenType::number, curr_line, num_pos, number});                
        } else if (var_characters.find(c) != string::npos) {
            go_forward();
            string var_name {c};
            int var_pos = curr_pos;
            optional<char> next_char = try_next();
            while (next_char.has_value() && var_characters.find(next_char.value()) != string::npos) {
                var_name += {next_char.value()};
                go_forward();
                next_char = try_next();
            }
            tokens.push_back({TokenType::variable, curr_line, var_pos, var_name});
        } else {
            cerr << "Buoya does not support that at line " + to_string(curr_line) +
            " and position " + to_string(curr_pos) + "\n";
            exit(10);
        }
    }
}


Lexer::Lexer(const path& filepath) {
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

vector<Token> Lexer::get_tokens() const {
    return tokens;
}