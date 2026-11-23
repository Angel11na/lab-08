#ifndef _TOKEN_HPP_
#define _TOKEN_HPP_

#include <string>

using namespace std;

enum class TokenType {
    Identifier,
    Number,
    Comma,
    Colon,
    NewLine,
    EndOfFile,
    LeftBracket,
    RightBracket
};
struct Token {
    TokenType type;
    string text;
    int line;
};

#endif