#pragma once
#include <iostream>
#include <vector>
#include <variant>
#include "structs.hpp"

using namespace std;


class Parser {
private:
    vector<Token> tokens;
    NodeProg* prog_node;
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
    string get_location() {
        return tokens[current].location(1);
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
            cerr << "Invalid term in an expression " + get_location() + "\n";
            exit(24);
        }
    };

    void look_for(TokenType type, string token_name) {
        if (next(type)) {
            current++;
        } else {
            cerr << "Expected " + token_name + " end " + get_location() + "\n";
            exit(22);
        }
    }

    NodeExpr* parse_expr(int min_prec = 0) {
        NodeExpr* expr_lhs = new NodeTerm;

        if (next(TokenType::expr_start)) {
            current++;
            expr_lhs = parse_expr();
            look_for(TokenType::expr_end, "expression");
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

    void assign(NodeStmtExpr* node_stmt_expr, string stmt_name) {
        if (next(TokenType::assign_expr)) {
            ++current;
            node_stmt_expr->node_expr = parse_expr();
                
        } else if (next(TokenType::assign_zero)) {
            NodeTermInt* node_term = new NodeTermInt;
            node_term->token = {TokenType::integer, -1, -1, "0"};
            node_stmt_expr->node_expr = node_term;
            current++;
        } else {
            cerr << "Invalid " + stmt_name + " " + get_location() + "\n";
            exit(26);
        }
    }

    NodeStmtExpr* parse_stmt_expr() {
        if (next(TokenType::ret)) {
            ++current;
            NodeStmtExprRet* exitnode = new NodeStmtExprRet;
            assign(exitnode, "return statement");
            return exitnode;
        } else if (next(TokenType::identifier)) {
            NodeStmtExprVar* varnode = new NodeStmtExprVar;
            varnode->ident = try_next().value();
            ++current;
            assign(varnode, "variable assignment");
            return varnode;
        } else {
            cerr << "Invalid statement " + get_location() + "\n";
            exit(25);
        }
    };

    TokenType after_statement() {
        optional<Token> next_token = try_next();
        if (next(TokenType::stmt_begin_end) || next(TokenType::stmt_middle)) {
            current++;
            return next_token.value().type;
        } else {
            cerr << "Expected statement end " + get_location() + "\n";
            exit(21);
        }
    };

    vector<NodeStmt*> parse_group_stmt_expr() {
        vector<NodeStmt*> stmts_expr;
        TokenType separator = TokenType::stmt_middle;
        while (separator == TokenType::stmt_middle) {
            stmts_expr.push_back(parse_stmt_expr());
            separator = after_statement();
        }
        return stmts_expr;
    }

    optional<NodeAfterIf*> parse_after_if() {
        TokenType type = after_scope();
        if (type == TokenType::scope_end) {
            return {};
        } else if (type == TokenType::els) {
            NodeAfterIfElse* after_if_else = new NodeAfterIfElse;
            after_if_else->scope = parse_scope();
            parse_after_if();
            return after_if_else;
        } else if (type == TokenType::elif_start) {
            NodeAfterIfElif* after_if_elif = new NodeAfterIfElif;
            after_if_elif->node_expr = parse_expr();
            look_for(TokenType::elif_end, "elif");
            after_if_elif->scope = parse_scope();
            after_if_elif->after_if = parse_after_if();
            return after_if_elif;
        }
    }

    vector<NodeStmt*> parse_stmts() {
        if (next(TokenType::stmt_begin_end)) {
            current++;
            return parse_group_stmt_expr();
        } else if (next(TokenType::if_start)) {
            current++;
            NodeStmtIf* stmt_if = new NodeStmtIf;
            stmt_if->node_expr = parse_expr();
            look_for(TokenType::if_end, "if");
            stmt_if->scope = parse_scope();
            stmt_if->after_if = parse_after_if();
            
            return {stmt_if};
        } else {
            cerr << "Expected statement begin " + get_location() + "\n";
            exit(20);
        }
    }

    TokenType after_scope() {
        optional<Token> next_token = try_next();
        if (next(TokenType::scope_end) || next(TokenType::elif_start) || next(TokenType::els)) {
            current++;
            return next_token.value().type;
        } else {
            cerr << "Expected scope end " + get_location() + "\n";
            exit(23);
        }
    }

    NodeScope* parse_scope() {
        NodeScope* scope = new NodeScope;
        while (try_next().has_value() && !next(TokenType::scope_end) && !next(TokenType::elif_start) && !next(TokenType::els)) {
            vector<NodeStmt*> stmts = parse_stmts();
            for (auto &stmt: stmts) {
                scope->stmts.push_back(stmt);
            }
        }
        return scope;
    }

    NodeProg* parse_prog() {
        NodeProg* prog = new NodeProg;
        while (try_next().has_value()) {
            vector<NodeStmt*> stmts = parse_stmts();
            for (auto &stmt: stmts) {
                prog->stmts.push_back(stmt);
            }
        }
        return prog;
    }


public:
    Parser(vector<Token> tokens) :tokens(tokens) {
        prog_node = parse_prog();
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