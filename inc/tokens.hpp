#pragma once

#include <iostream>
#include <optional>

using namespace std;

enum class TokenType {
    ret,
    number,
    stmt_begin_end,
    stmt_middle,
    variable,
    assign_zero,
    assign_expr,
    addition,
    substraction,
    multiplication,
    division,
    expr_start,
    expr_end,
    if_start,
    if_end,
    elif_start,
    elif_end,
    els,
    scope_end
};

struct Token {
    TokenType type;
    int line;
    int position;
    optional<string> value;

    string location(const bool end = 1) const {
        int new_pos = position;
        if (end) new_pos += (value.has_value() ? value.value().size() : 1);
        return "at line " + to_string(line) + " and position " + to_string(new_pos);
    }
};