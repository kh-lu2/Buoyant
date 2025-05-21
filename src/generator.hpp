#pragma once

#include "parser.hpp"
#include <unordered_map>

class Generator {
private:
    NodeProg root;
    string assembly;
    int stack_ptr = 0;

    struct Var {
        int stack_index;
    };

    unordered_map<string, Var> variables;

    void gen_expr(const NodeExpr& expr) {
        struct ExprVisitor {
            Generator* gen;
            ExprVisitor(Generator* gen) : gen(gen){}
            void operator() (const NodeExprInt& expr_int) {
                gen->assembly += "    mov rax, " + expr_int.integer.value.value() + "\n";
                gen->push("rax");
            }
            void operator() (const NodeExprIdent& expr_ident) {
                if (!gen->variables.contains(expr_ident.ident.value.value())) {
                    cerr << "No variable like that\n";
                    exit(10);
                }
                gen->push("qword [rsp + " + to_string((gen->stack_ptr - gen->variables[expr_ident.ident.value.value()].stack_index - 1) * 8) + "]");
            }
        };
        ExprVisitor visitor(this);
        visit(visitor, expr.var);
    }

    void gen_stmt(const NodeStmt& stmt) {
        struct StmtVisitor {
            Generator* gen;
            StmtVisitor(Generator* gen) : gen(gen) {}

            void operator()(const NodeStmtRet& stmt_exit) {
                gen->gen_expr(stmt_exit.node_expr);
                gen->assembly += "    mov rax, 60\n";
                gen->pop("rdi");
                gen->assembly += "    syscall\n";
            }

            void operator()(const NodeStmtVar& stmt_var) {
                if (gen->variables.contains(stmt_var.ident.value.value())) {
                    cerr << "Double declaration\n";
                    exit(9);
                }
                gen->variables[stmt_var.ident.value.value()] = Var{gen->stack_ptr};
                gen->gen_expr(stmt_var.node_expr);
            }
        };

        StmtVisitor visitor(this);
        visit(visitor, stmt.var);
    }


    void gen_prog() {
        assembly = "global _start\n_start:\n";

        for (auto &NodeStmt: root.stmts) {
            gen_stmt(NodeStmt);
        }

        assembly += "    mov rax, 60\n";
        assembly += "    mov rdi, 0\n";
        assembly += "    syscall\n";
    }

    void push(const string& reg) {
        assembly += "    push " + reg + "\n";
        stack_ptr++;
    }

    void pop(const string& reg) {
        assembly += "    pop " + reg + "\n";
        stack_ptr--;
    }

public:
    Generator(NodeProg root) : root(root){
        gen_prog();
    }

    string get_assembly() {
        return assembly;
    }
};