#ifndef _LEXER_HPP_
#define _LEXER_HPP_

#include <string>
#include <vector>

#include "token.hpp"

using namespace std;

class Lexer {
public:
    explicit Lexer(const string& source);

    vector<Token> tokenize();

private:
    string source;
    size_t position = 0;
    int line = 1;

    char peek() const;
    char advance();
    bool is_at_end() const;

    void skip_whitespace();

    Token identifier();
    Token number();
};

#endif