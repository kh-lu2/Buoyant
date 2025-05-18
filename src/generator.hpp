#pragma once

#include "parser.hpp"

class Generator {
private:
    NodeProg root;
    string assembly;

    string gen_expr(NodeExpr& expr) {
        struct ExprVisitor {
            void operator() (const NodeExprInt& expr_int) {

            }
            void operator() (const NodeExprIdent& expr_ident) {

            }
        };
        ExprVisitor visitor;
        visit(visitor, expr.var);
    }

    string gen_stmt(NodeStmt& stmt) {
        string tmp;

        struct StmtVisitor {
            string& tmp;

            void operator()(const NodeStmtRet& stmt_exit) {
                if (holds_alternative<NodeExprInt>(stmt_exit.node_expr.var)) {
                    auto expr_int = get<NodeExprInt>(stmt_exit.node_expr.var);
                    tmp += "    mov rax, 60\n";
                    tmp += "    mov rdi, " + expr_int.integer.value.value() + "\n";
                    tmp += "    syscall\n";
                } else {
                    std::cerr << "Expected integer expression in return!\n";
                    std::exit(9);
                }
            }

            void operator()(const NodeStmtVar& stmt_var) {
            }
        };

        StmtVisitor visitor{tmp};
        visit(visitor, stmt.var);

        return tmp;
    }


    void gen_prog() {
        assembly = "global _start\n_start:\n";

        for (auto &NodeStmt: root.stmts) {
            assembly += gen_stmt(NodeStmt);
        }

        assembly += "    mov rax, 60\n";
        assembly += "    mov rdi, 0\n";
        assembly += "    syscall\n";
    }
public:
    Generator(NodeProg root) : root(root){
        gen_prog();
    }

    string get_assembly() {
        return assembly;
    }
};