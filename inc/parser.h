#pragma once

#include <vector>
#include "tokens.hpp"
#include "astnodes.h"

using namespace std;

class Parser {
private:
    vector<Token> tokens;
    NodeProg* prog_node;
    int current = -1;

    optional<int> prec(const TokenType& type) const;

    string get_location() const;

    optional<Token> try_next() const;

    bool next(const TokenType& type) const;

    void look_for(const TokenType& type, const string& token_name);

    TokenType after_statement();

    TokenType after_scope();

    NodeTerm* parse_term();

    NodeExpr* parse_expr(const int& min_prec);

    void assign(NodeStmtSmpl* node_stmt_expr, const string& stmt_name);

    NodeStmtSmpl* parse_smpl_stmt();

    vector<NodeStmt*> parse_group_smpl_stmt();

    void assign_if(NodeIf* node_if, const TokenType& type, const string& token_name);

    optional<NodeAfterIf*> parse_after_if();

    vector<NodeStmt*> parse_stmts();

    NodeScope* parse_scope();

    NodeProg* parse_prog();

public:
    Parser(const vector<Token>& tokens);

    NodeProg get_node_prog() const;

    ~Parser();
};