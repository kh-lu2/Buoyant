#pragma once

#include <vector>
#include <filesystem>
#include <optional>
#include "tokens.hpp"
#include "astnodes.h"

using namespace std;
using filesystem::path;
const string var_characters = "@#$%";

class Lexer {
private:
    string source_file;
    vector<Token> tokens;
    int current = -1;
    int curr_line = 1;
    int curr_pos = 0;

    optional<char> try_next() const;

    bool next(const char& c) const;

    void go_forward();

    optional<TokenType> cast_single(const char& c) const;

    void tokenize();

public:
    Lexer(const path& filepath);

    vector<Token> get_tokens() const;
};