#ifndef _ASSEMBLER_HPP_
#define _ASSEMBLER_HPP_

#include <string>
#include <unordered_map>
#include <vector>

#include "token.hpp"

using namespace std;

class Assembler {
public:
    explicit Assembler(const vector<Token>& tokens);

    vector<unsigned char> assemble();

private:
    vector<Token> tokens;
    size_t position = 0;
    size_t address = 0;

    unordered_map<string, size_t> labels;
    vector<unsigned char> code;

    unsigned int parse_number(const string& text) const;

    void first_pass();
    void second_pass();
};

#endif