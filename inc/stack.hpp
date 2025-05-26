#pragma once

#include <iostream>
#include <stack>
#include <unordered_map>

using namespace std;

struct Stack {
    int stack_ptr = 0;
    int label_cnt = 0;
    unordered_map<string, int> variables;
    stack<string> variable_stack;
    stack<int> scope_starts;

    string push(const string& reg) {
        stack_ptr++;
        return "    push " + reg + "\n";
    }

    string pop(const string& reg) {
        stack_ptr--;
        return "    pop " + reg + "\n";
    }    

    string create_label() {
        return "label" + to_string(label_cnt++);
    }
};