#include "lexer.hpp"

#include <iostream>

using namespace std;

int main() {
    string source =
        "LOADI_A 25\n"
        "LOADI_B 0x1A\n"
        "LOADI_A 0b1010\n";

    Lexer lexer(source);
    vector<Token> tokens = lexer.tokenize();

    for (const Token& token : tokens) {
        cout << "Line: " << token.line
             << " | Type: " << static_cast<int>(token.type)
             << " | Text: " << token.text << '\n';
    }

    return 0;
}