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

    optional<int> prec(TokenType type) {
        if (type == TokenType::addition || type == TokenType::substraction) return 0;
        if (type == TokenType::multiplication || type == TokenType::division) return 1;
        return {};
    }

    optional<Token> try_next() {
        if (current + 1 >= tokens.size()) return {};
        return tokens[current + 1];
    }

    bool next(TokenType type) {
        return try_next().has_value() && try_next()->type == type;
    }

    NodeTerm* parse_term() {
        if (next(TokenType::integer)) {
            NodeTermInt* node_term = new NodeTermInt;
            node_term->token = tokens[++current];
            return node_term;
        } else if (next(TokenType::identifier)) {
            NodeTermIdent* node_term = new NodeTermIdent;
            node_term->token = tokens[++current];
            return node_term;
        } else {
            cerr << "Invalid expression\n";
            exit(5);
        }
    };

    NodeExpr* parse_expr(int min_prec = 0) {
        //NodeTerm* term = parse_term();
        NodeTerm* term;
        if (next(TokenType::integer)) {
            NodeTermInt* node_term = new NodeTermInt;
            node_term->token = tokens[++current];
            term = node_term;
        } else if (next(TokenType::identifier)) {
            NodeTermIdent* node_term = new NodeTermIdent;
            node_term->token = tokens[++current];
            term = node_term;
        } else {
            cerr << "Invalid expression\n";
            exit(5);
        }

        NodeExpr* expr_lhs = term;

        while (true) {
            optional<Token> current_token = try_next();
            if (!current_token.has_value()) break;
            auto curr_prec = prec(current_token->type);
            if (!curr_prec.has_value() || curr_prec < min_prec) break;

            TokenType type = tokens[++current].type;
            int next_min_prec = curr_prec.value() + 1;
            NodeExpr* expr_rhs = parse_expr(next_min_prec);

            NodeBinExpr* expr = new NodeBinExpr;

            if (type ==TokenType::addition) {
                NodeBinExprAdd* add_expr = new NodeBinExprAdd;
                add_expr->lhs = expr_lhs;
                add_expr->rhs = expr_rhs;
                expr = add_expr;
            } else if (type == TokenType::substraction) {
                NodeBinExprSub* sub_expr = new NodeBinExprSub;
                sub_expr->lhs = expr_lhs;
                sub_expr->rhs = expr_rhs;
                expr = sub_expr;
            } else if (type == TokenType::multiplication) {
                NodeBinExprMul* mul_expr = new NodeBinExprMul;
                mul_expr->lhs = expr_lhs;
                mul_expr->rhs = expr_rhs;
                expr = mul_expr;
            } else if (type == TokenType::division) {
                NodeBinExprDiv* div_expr = new NodeBinExprDiv;
                div_expr->lhs = expr_lhs;
                div_expr->rhs = expr_rhs;
                expr = div_expr;
            }
            expr->lhs = expr_lhs;
            expr->rhs = expr_rhs;
            
            expr_lhs = expr;    
        }

        return expr_lhs;
    }

    NodeStmt* parse_stmt() {
        if (next(TokenType::ret)) {
            ++current;
            NodeStmtRet* exitnode = new NodeStmtRet;

            if (next(TokenType::assign_expr)) {
                ++current;
                exitnode->node_expr = parse_expr();
                
            } else if (next(TokenType::assign_zero)) {
                NodeTermInt* node_term = new NodeTermInt;
                node_term->token = {TokenType::integer, "0"};
                exitnode->node_expr = node_term;
                current++;
            } else {
                cerr << "Invalid return statement\n";
                exit(11);
            }

            return exitnode;
        } else if (next(TokenType::identifier)) {
            NodeStmtVar* varnode = new NodeStmtVar;
            varnode->ident = try_next().value();
            ++current;
            if (next(TokenType::assign_expr)) {
                ++current;
                varnode->node_expr = parse_expr();
            } else if (next(TokenType::assign_zero)) {
                NodeTermInt* node_term = new NodeTermInt;
                node_term->token = {TokenType::integer, "0"};
                varnode->node_expr = node_term;
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
        optional<Token> next_token = try_next();
        if (next(TokenType::stmt_begin_end)) {
            current++;
            return next_token.value();
        } else if (next(TokenType::stmt_middle)) {
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