
#include "lexer.hpp"

#include <cctype>
#include <stdexcept>

using namespace std;

Lexer::Lexer(const string& source) : source(source) {}

char Lexer::peek() const {
    if (is_at_end())
        return '\0';

    return source[position];
}

char Lexer::advance() {
    if (is_at_end())
        return '\0';

    return source[position++];
}

bool Lexer::is_at_end() const {
    return position >= source.size();
}

void Lexer::skip_whitespace() {
    while (!is_at_end()) {
        char c = peek();

        if (c == ' ' || c == '\t' || c == '\r')
            advance();
        else
            break;
    }
}

Token Lexer::identifier() {
    size_t start = position;

    while (!is_at_end()) {
        char c = peek();

        if (isalnum(static_cast<unsigned char>(c)) || c == '_')
            advance();
        else
            break;
    }

    return {TokenType::Identifier, source.substr(start, position - start), line};
}

Token Lexer::number() {
    size_t start = position;

    if (peek() == '0' && position + 1 < source.size()
        && (source[position + 1] == 'x' || source[position + 1] == 'X')) {
        advance();
        advance();

        size_t digits = position;

        while (!is_at_end() && isxdigit(static_cast<unsigned char>(peek())))
            advance();

        if (position == digits)
            throw runtime_error("Line " + to_string(line) + ": invalid hexadecimal number");
    }
    else if (peek() == '0' && position + 1 < source.size()
             && (source[position + 1] == 'b' || source[position + 1] == 'B')) {
        advance();
        advance();

        size_t digits = position;

        while (!is_at_end() && (peek() == '0' || peek() == '1'))
            advance();

        if (position == digits)
            throw runtime_error("Line " + to_string(line) + ": invalid binary number");
    }
    else {
        while (!is_at_end() && isdigit(static_cast<unsigned char>(peek())))
            advance();
    }

    if (!is_at_end()) {
        char c = peek();

        if (isalnum(static_cast<unsigned char>(c)) || c == '_')
            throw runtime_error("Line " + to_string(line) + ": malformed number");
    }

    return {TokenType::Number, source.substr(start, position - start), line};
}

vector<Token> Lexer::tokenize() {
    vector<Token> tokens;

    while (!is_at_end()) {
        skip_whitespace();

        if (is_at_end())
            break;

        char c = peek();

        if (c == '\n') {
            advance();
            tokens.push_back({TokenType::NewLine, "\n", line});
            ++line;
            continue;
        }

        if (c == ',') {
            advance();
            tokens.push_back({TokenType::Comma, ",", line});
            continue;
        }

        if (c == ':') {
            advance();
            tokens.push_back({TokenType::Colon, ":", line});
            continue;
        }

        if (c == '[') {
            advance();
            tokens.push_back({TokenType::LeftBracket, "[", line});
            continue;
        }

        if (c == ']') {
            advance();
            tokens.push_back({TokenType::RightBracket, "]", line});
            continue;
        }

        if (c == ';') {
            while (!is_at_end() && peek() != '\n')
                advance();

            continue;
        }

        if (isdigit(static_cast<unsigned char>(c))) {
            tokens.push_back(number());
            continue;
        }

        if (isalpha(static_cast<unsigned char>(c)) || c == '_') {
            tokens.push_back(identifier());
            continue;
        }

        throw runtime_error("Line " + to_string(line) + ": unexpected character");
    }

    tokens.push_back({TokenType::EndOfFile, "", line});

    return tokens;
}
