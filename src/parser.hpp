#pragma once
#include <iostream>
#include <vector>
#include <variant>
#include "structs.hpp"

using namespace std;


class Parser {
private:
    vector<Token> tokens;
    NodeProg prog_node;
    int current = 0;

    optional<Token> next() {
        if (current + 1 >= tokens.size()) return {};
        return tokens[current + 1];
    }

    NodeExpr* parse_expr() {
        optional<Token> next_token = next();
        if (next_token.has_value() && next_token.value().type == TokenType::integer) {
            NodeExprInt* node_expr = new NodeExprInt;
            node_expr->token = tokens[++current];
            return node_expr;
        } else if (next_token.has_value() &&  next_token.value().type == TokenType::identifier) {
            NodeExprIdent* node_expr = new NodeExprIdent;
            node_expr->token = tokens[++current];
            return node_expr;
        } else {
            cerr << "Invalid expression\n";
            exit(5);
        }
    };

    NodeStmt* parse_stmt() {
        if (next().has_value() && next().value().type == TokenType::ret) {
            ++current;
            NodeStmtRet* exitnode = new NodeStmtRet;

            if (next().has_value() && next().value().type == TokenType::assign_expr) {
                ++current;
                exitnode->node_expr = parse_expr();
                
            } else if (next().has_value() && next().value().type == TokenType::assign_zero) {
                NodeExprInt* node_expr = new NodeExprInt;
                node_expr->token = {TokenType::integer, "0"};
                exitnode->node_expr = node_expr;
                current++;
            } else {
                cerr << "Invalid return statement\n";
                exit(11);
            }

            return exitnode;
        } else if (next().has_value() && next().value().type == TokenType::identifier) {
            NodeStmtVar* varnode = new NodeStmtVar;
            varnode->ident = next().value();
            ++current;
            if (next().has_value() && next().value().type == TokenType::assign_expr) {
                ++current;
                varnode->node_expr = parse_expr();
            } else if (next().has_value() && next().value().type == TokenType::assign_zero) {
                NodeExprInt* node_expr = new NodeExprInt;
                node_expr->token = {TokenType::integer, "0"};
                varnode->node_expr = node_expr;
                current++;
            } else {
                cerr << "Invalid variable assignment\n";
                exit(12);
            }
            return varnode;
        } else {
            cerr << "Invalid statement\n";
            exit(13);
        }
    };

    Token after_statement() {
        optional<Token> next_token = next();
        if (next_token.has_value() && next_token.value().type == TokenType::stmt_begin_end) {
            current++;
            return next_token.value();
        } else if (next_token.has_value() && next_token.value().type == TokenType::stmt_middle) {
            current++;
            return next_token.value();
        } else {
            cerr << "Expected statement end\n";
            exit(6);
        }
    };

    void parse() {
        while (current < tokens.size()) {
            Token& token = tokens[current];

            if (token.type == TokenType::stmt_begin_end) {
                Token separator = Token{TokenType::stmt_middle};
                while (separator.type == TokenType::stmt_middle) {
                    prog_node.stmts.push_back(parse_stmt());
                    separator = after_statement();
                }
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

    ~Parser() {
        for (auto &stmt: prog_node.stmts) {
            delete stmt;
        }
    }
};