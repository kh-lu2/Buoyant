#pragma once
#include <iostream>
#include <vector>
#include <variant>
#include "../inc/astnodes.h"
#include "../inc/tokens.hpp"
#include "../inc/parser.h"

using namespace std;

optional<int> Parser::prec(const TokenType& type) const {
    switch (type) {
        case TokenType::addition:
        case TokenType::substraction:
            return 0;
        case TokenType::multiplication:
        case TokenType::division:
            return 1;
        default:
            return {};
    }
}

string Parser::get_location() const {
    return tokens[current].location(1);
}

optional<Token> Parser::try_next() const {
    if (current + 1 >= tokens.size()) return {};
    return tokens[current + 1];
}

bool Parser::next(const TokenType& type) const {
    return try_next().has_value() && try_next()->type == type;
}

void Parser::look_for(const TokenType& type, const string& token_name) {
    if (!next(type)) {
        cerr << "Expected " + token_name + " end " + get_location() + "\n";
        exit(22);
    }
    current++;
}

TokenType Parser::after_statement() {
    if (!next(TokenType::stmt_begin_end) && !next(TokenType::stmt_middle)) {
        cerr << "Expected statement end " + get_location() + "\n";
        exit(21);
    }
  
    return tokens[++current].type;
};

TokenType Parser::after_scope() {
    if (!next(TokenType::scope_end) && ! next(TokenType::elif_start) && !next(TokenType::els)) {
        cerr << "Expected scope end " + get_location() + "\n";
        exit(23);
    }
    return tokens[++current].type;
}

unique_ptr<NodeTerm> Parser::parse_term() {
    if (!next(TokenType::number) && !next(TokenType::variable)) {
        cerr << "Invalid term in an expression " + get_location() + "\n";
        exit(24);
    }

    unique_ptr<NodeTerm> node_term;
    if (next(TokenType::number)) node_term = make_unique<NodeTermNum>();
    else node_term = make_unique<NodeTermVar>();

    node_term->token = tokens[++current];
    return node_term;
};

unique_ptr<NodeExpr> Parser::parse_expr(const int& min_prec = 0) {
    unique_ptr<NodeExpr> expr_lhs;

    if (next(TokenType::expr_start)) {
        current++;
        expr_lhs = parse_expr();
        look_for(TokenType::expr_end, "expression");
    } else expr_lhs = parse_term();

    while (true) {
        auto current_token = try_next();
        if (!current_token.has_value()) break;
        auto curr_prec = prec(current_token->type);
        if (!curr_prec.has_value() || curr_prec < min_prec) break;

        TokenType type = tokens[++current].type;
        int next_min_prec = curr_prec.value() + 1;
        unique_ptr<NodeExpr> expr_rhs = parse_expr(next_min_prec);

        unique_ptr<NodeMathExpr> expr;

        if (type == TokenType::addition) expr = make_unique<NodeMathExprAdd>();
        else if (type == TokenType::substraction) expr = make_unique<NodeMathExprSub>();
        else if (type == TokenType::multiplication) expr = make_unique<NodeMathExprMul>();
        else expr = make_unique<NodeMathExprDiv>();

        expr->lhs = move(expr_lhs);
        expr->rhs = move(expr_rhs);
        
        expr_lhs = move(expr);    
    }

    return expr_lhs;
}

void Parser::assign(shared_ptr<NodeStmtSmpl> node_stmt_expr, const string& stmt_name) {
    if (next(TokenType::assign_expr)) {
        current++;
        node_stmt_expr->node_expr = parse_expr();
    } else if (next(TokenType::assign_zero)) {
        current++;
        unique_ptr<NodeTermNum> node_term(new NodeTermNum);
        node_term->token = {TokenType::number, -1, -1, "0"};
        node_stmt_expr->node_expr = move(node_term);
    } else {
        cerr << "Invalid " + stmt_name + " " + get_location() + "\n";
        exit(26);
    }
}

shared_ptr<NodeStmtSmpl> Parser::parse_smpl_stmt() {
    if (next(TokenType::ret)) {
        current++;
        shared_ptr<NodeStmtSmplRet> exitnode(new NodeStmtSmplRet);
        assign(exitnode, "return statement");
        return exitnode;
    } else if (next(TokenType::variable)) {
        shared_ptr<NodeStmtSmplVar> varnode(new NodeStmtSmplVar);
        varnode->ident = try_next().value();
        current++;
        assign(varnode, "variable assignment");
        return varnode;
    } else {
        cerr << "Invalid statement " + get_location() + "\n";
        exit(25);
    }
};

vector<shared_ptr<NodeStmt>> Parser::parse_group_smpl_stmt() {
    vector<shared_ptr<NodeStmt>> stmts_expr;
    TokenType separator = TokenType::stmt_middle;
    while (separator == TokenType::stmt_middle) {
        stmts_expr.push_back(parse_smpl_stmt());
        separator = after_statement();
    }
    return stmts_expr;
}

void Parser::assign_if(shared_ptr<NodeIf> node_if, const TokenType& type, const string& token_name) {
    node_if->node_expr = parse_expr();
    look_for(type, token_name);
    node_if->scope = parse_scope();
    node_if->after_if = parse_after_if();
}

optional<shared_ptr<NodeAfterIf>> Parser::parse_after_if() {
    TokenType type = after_scope();
    if (type == TokenType::scope_end) return {};
    
    if (type == TokenType::els) {
        shared_ptr<NodeAfterIfElse> after_if_else(new NodeAfterIfElse);
        after_if_else->scope = parse_scope();
        if (after_scope() != TokenType::scope_end){
            cerr << "Invalid scope end " + get_location() + "\n";
            exit(27);
        }           
        return after_if_else;
    } else if (type == TokenType::elif_start) {
        shared_ptr<NodeAfterIfElif> after_if_elif(new NodeAfterIfElif);
        assign_if(after_if_elif, TokenType::elif_end, "elif");
        return after_if_elif;
    }
}

vector<shared_ptr<NodeStmt>> Parser::parse_stmts() {
    if (next(TokenType::stmt_begin_end)) {
        current++;
        return parse_group_smpl_stmt();
    } else if (next(TokenType::if_start)) {
        current++;
        shared_ptr<NodeStmtIf> stmt_if(new NodeStmtIf);
        assign_if(stmt_if, TokenType::if_end, "if");
        return {stmt_if};
    } else {
        cerr << "Expected statement begin " + get_location() + "\n";
        exit(20);
    }
}

unique_ptr<NodeScope> Parser::parse_scope() {
    unique_ptr<NodeScope> scope(new NodeScope);
    while (try_next().has_value() && !next(TokenType::scope_end) && !next(TokenType::elif_start) && !next(TokenType::els)) {
        vector<shared_ptr<NodeStmt>> stmts = parse_stmts();
        for (auto &stmt: stmts) scope->stmts.push_back(stmt);
    }
    return scope;
}

unique_ptr<NodeProg> Parser::parse_prog() {
    unique_ptr<NodeProg> prog(new NodeProg);
    while (try_next().has_value()) {
        vector<shared_ptr<NodeStmt>> stmts = parse_stmts();
        for (auto &stmt: stmts) prog->stmts.push_back(stmt);
    }
    return prog;
}

Parser::Parser(const vector<Token>& tokens) : tokens(tokens) {
    prog_node = parse_prog();
}

NodeProg Parser::get_node_prog() const {
    return *prog_node;
}