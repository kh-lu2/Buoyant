#include <iostream>
#include <fstream>
#include <filesystem>
#include <optional>
#include <vector>

using namespace std;
using filesystem::path;

enum class TokenType {
    ret,
    integer,
    statement_end
};

struct Token {
    TokenType type;
    optional<int> value;
};


int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Wrong usage. Should be: buoyant <source.bya>\n";
        return 1;
    }

    path filepath(argv[1]);
    if (filepath.extension() != ".bya") {
        cerr << "Wrong file extention! Should be .bya\n";
        return 2;
    }

    ifstream source(filepath);
    if (source.fail()) {
        cerr << "Something wrong with file or filepath...\n";
        return 3;
    }

    vector<Token> tokens;
    string str;
    while (source >> str) {
        try {
            int value = stoi(str);
            tokens.push_back({TokenType::integer, value});
        }
        catch (std::invalid_argument const& ex) {
            if (str == "&") {
                tokens.push_back({TokenType::ret});
            } else if (str == ">") {
                tokens.push_back({TokenType::statement_end});
            } else {
                cerr << "Buoya does not support that\n";
                return 4;
            }
        }
    }

    cout << argv[1] << "\n";
    return 0;
}