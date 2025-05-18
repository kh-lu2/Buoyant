#pragma once
#include <iostream>
#include <vector>
#include <variant>
#include "lexer.hpp"

using namespace std;

struct NodeExprInt {
    Token integer;
};

struct NodeExprIdent {
    Token ident;
};

struct NodeExpr {
    variant<NodeExprInt, NodeExprIdent> var;
};

struct NodeStmtRet {
    NodeExpr node_expr;
};

struct NodeStmtVar {
    Token ident;
    NodeExpr node_expr;
};

struct NodeStmt {
    variant<NodeStmtRet, NodeStmtVar> var;
};

struct NodeProg {
    vector<NodeStmt> stmts;
};

class Parser {
private:
    vector<Token> tokens;
    NodeProg prog_node;
    int current = 0;

    optional<Token> next() {
        if (current + 1 > tokens.size()) return {};
        return tokens[current + 1];
    }

    optional<NodeExpr> parse_expr() {
        optional<Token> next_token = next();
        if (next_token.has_value() && next_token.value().type == TokenType::integer) {
            return NodeExpr{NodeExprInt{tokens[++current]}};
        } else if (next_token.has_value() &&  next_token.value().type == TokenType::ident) {
            return NodeExpr{NodeExprIdent{tokens[++current]}};
        } else {
            return {};
        }
    };

    optional<NodeStmt> parse_stmt() {
        if (next().has_value() && next().value().type == TokenType::ret) {
            ++current;
            NodeStmtRet exitnode;
            if (auto node_expr = parse_expr()) {
                exitnode.node_expr = node_expr.value();
            } else {
                cerr << "Invalid expression\n";
                exit(5);
            }
            look_for_stmt_end();

            return NodeStmt{NodeStmtRet{exitnode}};
        } else if (next().has_value() && next().value().type == TokenType::ident) {
            ++current;
            NodeStmtVar varnode;
            varnode.ident = next().value();
            if (auto node_expr = parse_expr()) {
                varnode.node_expr = node_expr.value();
            } else {
                cerr << "Invalid expression\n";
                exit(5);
            }
            look_for_stmt_end();
            return NodeStmt{NodeStmtVar{varnode}};
        } else {
            return {};
        }
    };

    void look_for_stmt_end() {
        optional<Token> next_token = next();
        if (next_token.has_value() && next_token.value().type == TokenType::stmt_end) {
            current++;
        } else {
            cerr << "Expected statement end\n";
            exit(6);
        }
    };

    void parse() {
        while (current < tokens.size()) {
            Token& token = tokens[current];

            if (token.type == TokenType::stmt_begin) {
                prog_node.stmts.push_back(parse_stmt().value());
            } else {
                cerr << "Buoya doesn't understand that\n";
                exit(8);
            }
            current++;
        }
    }
public:
    Parser(vector<Token> tokens) :tokens(tokens) {
        parse();
    } 

    NodeProg get_node_prog() {
        return prog_node;
    }
};