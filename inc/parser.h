#pragma once

#include <vector>
#include <memory>
#include "tokens.hpp"
#include "astnodes.h"

using namespace std;

class Parser {
private:
    vector<Token> tokens;
    unique_ptr<NodeProg> prog_node;
    int current = -1;

    optional<int> prec(const TokenType& type) const;

    string get_location() const;

    optional<Token> try_next() const;

    bool next(const TokenType& type) const;

    void look_for(const TokenType& type, const string& token_name);

    TokenType after_statement();

    TokenType after_scope();

    unique_ptr<NodeTerm> parse_term();

    unique_ptr<NodeExpr> parse_expr(const int& min_prec);

    void assign(shared_ptr<NodeStmtSmpl> node_stmt_expr, const string& stmt_name);

    shared_ptr<NodeStmtSmpl> parse_smpl_stmt();

    vector<shared_ptr<NodeStmt>> parse_group_smpl_stmt();

    void assign_if(shared_ptr<NodeIf> node_if, const TokenType& type, const string& token_name);

    optional<shared_ptr<NodeAfterIf>> parse_after_if();

    vector<shared_ptr<NodeStmt>> parse_stmts();

    unique_ptr<NodeScope> parse_scope();

    unique_ptr<NodeProg> parse_prog();

public:
    Parser(const vector<Token>& tokens);

    NodeProg get_node_prog() const;
};