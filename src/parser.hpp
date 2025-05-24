#pragma once
#include <iostream>
#include <vector>
#include <variant>
#include "structs.hpp"

using namespace std;


class Parser {
private:
    vector<Token> tokens;
    NodeProg* prog_node = new NodeProg;
    int current = -1;

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

    void look_for_expr_end() {
        if (next(TokenType::expr_end)) {
            current++;
        } else {
            cerr << "No expression end\n";
            exit(14);
        }
    }

    NodeExpr* parse_expr(int min_prec = 0) {
        NodeExpr* expr_lhs = new NodeTerm;

        if (next(TokenType::expr_start)) {
            current++;
            expr_lhs = parse_expr();
            look_for_expr_end();
        } else {        
            expr_lhs = parse_term();
        }

        while (true) {
            optional<Token> current_token = try_next();
            if (!current_token.has_value()) break;
            auto curr_prec = prec(current_token->type);
            if (!curr_prec.has_value() || curr_prec < min_prec) break;

            TokenType type = tokens[++current].type;
            int next_min_prec = curr_prec.value() + 1;
            NodeExpr* expr_rhs = parse_expr(next_min_prec);

            NodeBinExpr* expr;

            if (type == TokenType::addition) {
                expr = new NodeBinExprAdd;
            } else if (type == TokenType::substraction) {
                expr = new NodeBinExprSub;
            } else if (type == TokenType::multiplication) {
                expr = new NodeBinExprMul;
            } else if (type == TokenType::division) {
                expr = new NodeBinExprDiv;
            }

            expr->lhs = expr_lhs;
            expr->rhs = expr_rhs;
            
            expr_lhs = expr;    
        }

        return expr_lhs;
    }

    NodeStmt* parse_stmt_expr() {
        if (next(TokenType::ret)) {
            ++current;
            NodeStmtExprRet* exitnode = new NodeStmtExprRet;

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
            NodeStmtExprVar* varnode = new NodeStmtExprVar;
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

    void parse_group_stmt_expr(NodeScope* scope) {
        Token separator = Token{TokenType::stmt_middle};
        while (separator.type == TokenType::stmt_middle) {
            scope->stmts.push_back(parse_stmt_expr());
            separator = after_statement();
        }
    }

    void look_for_if_end() {
        if (next(TokenType::if_end)) {
            current++;
        } else {
            cerr << "No if end\n";
            exit(17);
        }
    }

    void parse_stmts(NodeScope* scope) {
        if (next(TokenType::stmt_begin_end)) {
            current++;
            parse_group_stmt_expr(scope);
        } else if (next(TokenType::if_start)) {
            current++;
            NodeStmtIf* stmt_if = new NodeStmtIf;
            stmt_if->node_expr = parse_expr();
            look_for_if_end();
            NodeScope* new_scope = new NodeScope;
            parse_scope(new_scope);
            stmt_if->scope = new_scope;
            scope->stmts.push_back(stmt_if);
        } else {
            cerr << "Buoya doesn't understand that\n";
            exit(8);
        }
    }

    void look_for_scope_end() {
        if (next(TokenType::scope_end)) {
            current++;
        } else {
            cerr << "No scope end\n";
            exit(15);
        }
    }

    void parse_scope(NodeScope* scope) {
        while (try_next().has_value() && !next(TokenType::scope_end)) {
            parse_stmts(scope);
        }
        look_for_scope_end();
    }

    void parse(NodeScope* scope) {
        while (try_next().has_value()) {
            parse_stmts(scope);
        }
    }


public:
    Parser(vector<Token> tokens) :tokens(tokens) {
        parse(prog_node);
    } 

    NodeProg get_node_prog() {
        return *prog_node;
    }

    ~Parser() {
        for (auto &stmt: prog_node->stmts) {
            delete stmt;
        }
    }
};