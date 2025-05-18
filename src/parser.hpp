#pragma once
#include <iostream>
#include <vector>
#include "lexer.hpp"

using namespace std;

struct NodeExpr {
    Token integer;
};
struct NodeExit {
    NodeExpr expr;
};

class Parser {
private:
    vector<Token> tokens;
    NodeExit exit_node;
    int current = 0;

    optional<Token> next() {
        if (current + 1 > tokens.size()) return {};
        return tokens[current + 1];
    }

    optional<NodeExpr> parse_expr() {
        optional<Token> next_token = next();
        if (next_token.has_value() && next_token.value().type == TokenType::integer) {
            return NodeExpr{tokens[++current]};
        } else {
            return {};
        }
    };

    void look_for_stmt_end() {
        optional<Token> next_token = next();
        if (next_token.has_value() && next_token.value().type == TokenType::statement_end) {
            current++;
        } else {
            cerr << "Expected statement end\n";
            exit(6);
        }
    }

    void parse() {
        while (current < tokens.size()) {
            const Token& token = tokens[current];
            if (token.type == TokenType::ret) {
                if (auto node_expr = parse_expr()) {
                    exit_node.expr = node_expr.value();
                } else {
                    cerr << "Invalid expression\n";
                    exit(5);
                }

                look_for_stmt_end();
            }
            current++;
        }
    }
public:
    Parser(vector<Token> tokens) :tokens(tokens) {
        parse();
    } 

    NodeExit get_node_exit() {
        return exit_node;
    }
};