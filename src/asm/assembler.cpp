
#include "assembler.hpp"

#include <limits>
#include <stdexcept>

using namespace std;

Assembler::Assembler(const vector<Token>& tokens) : tokens(tokens) {}

unsigned int Assembler::parse_number(const string& text) const {
    try {
        size_t position = 0;
        unsigned long long value = 0;

        if (text.size() > 2 && text[0] == '0'
            && (text[1] == 'x' || text[1] == 'X')) {
            value = stoull(text.substr(2), &position, 16);

            if (position != text.size() - 2)
                throw runtime_error("Invalid number");
        }
        else if (text.size() > 2 && text[0] == '0'
                 && (text[1] == 'b' || text[1] == 'B')) {
            value = stoull(text.substr(2), &position, 2);

            if (position != text.size() - 2)
                throw runtime_error("Invalid number");
        }
        else {
            value = stoull(text, &position, 10);

            if (position != text.size())
                throw runtime_error("Invalid number");
        }

        if (value > numeric_limits<unsigned int>::max())
            throw runtime_error("Number is too large");

        return static_cast<unsigned int>(value);
    }
    catch (const exception&) {
        throw runtime_error("Invalid number: " + text);
    }
}

void Assembler::first_pass() {
    position = 0;
    address = 0;
    labels.clear();

    while (tokens[position].type != TokenType::EndOfFile) {
        if (tokens[position].type == TokenType::NewLine) {
            ++position;
            continue;
        }

        if (tokens[position].type == TokenType::Identifier
            && tokens[position + 1].type == TokenType::Colon) {
            string label = tokens[position].text;

            if (labels.count(label))
                throw runtime_error("Duplicate label: " + label);

            labels[label] = address;
            position += 2;
        }

        if (tokens[position].type == TokenType::NewLine) {
            ++position;
            continue;
        }

        if (tokens[position].type == TokenType::EndOfFile)
            break;

        string instruction = tokens[position].text;
        size_t size = 1;

        if (instruction == "LOADI")
            size = 2;

        else if (instruction == "OUTS" || instruction == "LOADH"
                 || instruction == "JMP" || instruction == "JZ"
                 || instruction == "JNZ" || instruction == "JC"
                 || instruction == "JNC" || instruction == "CALL")
            size = 3;

        else if (instruction == "LOAD"
                 || instruction == "STORE") {
            size = 3;

            for (size_t i = position; i < tokens.size()
                 && tokens[i].type != TokenType::NewLine; ++i) {
                if (tokens[i].type == TokenType::LeftBracket
                    && i + 1 < tokens.size()
                    && tokens[i + 1].text == "H") {
                    size = 1;
                    break;
                }
            }
        }

        address += size;

        if (address > 0x800)
            throw runtime_error("Program is too large");

        while (tokens[position].type != TokenType::NewLine
               && tokens[position].type != TokenType::EndOfFile)
            ++position;
    }
}

void Assembler::second_pass() {
    position = 0;
    code.clear();

    while (tokens[position].type != TokenType::EndOfFile) {
        if (tokens[position].type == TokenType::NewLine) {
            ++position;
            continue;
        }

        if (tokens[position].type == TokenType::Identifier
            && tokens[position + 1].type == TokenType::Colon)
            position += 2;

        if (tokens[position].type == TokenType::NewLine
            || tokens[position].type == TokenType::EndOfFile)
            continue;

        vector<string> words;

        while (tokens[position].type != TokenType::NewLine
               && tokens[position].type != TokenType::EndOfFile) {
            if (tokens[position].type == TokenType::Identifier
                || tokens[position].type == TokenType::Number)
                words.push_back(tokens[position].text);

            ++position;
        }

        if (words.empty())
            continue;

        string instruction = words[0];

        auto get_value = [&](const string& text) -> unsigned int {
            if (labels.count(text))
                return static_cast<unsigned int>(labels[text]);

            return parse_number(text);
        };

        auto add_address = [&](unsigned int value) {
            code.push_back(value & 0xFF);
            code.push_back((value >> 8) & 0xFF);
        };

        if (instruction == "HALT") code.push_back(0x00);
        else if (instruction == "NOP") code.push_back(0x01);
        else if (instruction == "OUT") code.push_back(0x02);
        else if (instruction == "OUTN") code.push_back(0x03);

        else if (instruction == "OUTS") {
            code.push_back(0x04);
            add_address(get_value(words[1]));
        }

        else if (instruction == "ADD") code.push_back(0x10);
        else if (instruction == "SUB") code.push_back(0x11);
        else if (instruction == "AND") code.push_back(0x12);
        else if (instruction == "OR") code.push_back(0x13);
        else if (instruction == "XOR") code.push_back(0x14);
        else if (instruction == "NOT") code.push_back(0x15);
        else if (instruction == "SHL") code.push_back(0x16);
        else if (instruction == "SHR") code.push_back(0x17);
        else if (instruction == "INC") code.push_back(0x18);
        else if (instruction == "DEC") code.push_back(0x19);
        else if (instruction == "CMP") code.push_back(0x1A);
        else if (instruction == "MUL") code.push_back(0x1B);

        else if (instruction == "LOADI") {
            if (words[1] == "A")
                code.push_back(0x20);
            else if (words[1] == "B")
                code.push_back(0x21);
            else
                throw runtime_error("Invalid register");

            unsigned int value = get_value(words[2]);

            if (value > 255)
                throw runtime_error("Value is too large");

            code.push_back(value);
        }

        else if (instruction == "LOADH") {
            code.push_back(0x28);
            add_address(get_value(words[2]));
        }

        else if (instruction == "LOAD") {
            if (words[2] == "H")
                code.push_back(0x29);
            else {
                code.push_back(words[1] == "A" ? 0x22 : 0x23);
                add_address(get_value(words[2]));
            }
        }

        else if (instruction == "STORE") {
            if (words[1] == "H")
                code.push_back(0x2A);
            else {
                code.push_back(words[2] == "A" ? 0x24 : 0x25);
                add_address(get_value(words[1]));
            }
        }

        else if (instruction == "MOV") {
            if (words[1] == "A" && words[2] == "B")
                code.push_back(0x26);
            else if (words[1] == "B" && words[2] == "A")
                code.push_back(0x27);
            else
                throw runtime_error("Invalid MOV registers");
        }

        else if (instruction == "INCH") code.push_back(0x2B);
        else if (instruction == "DECH") code.push_back(0x2C);
        else if (instruction == "HLOW") code.push_back(0x2D);

        else if (instruction == "JMP") {
            code.push_back(0x30);
            add_address(get_value(words[1]));
        }
        else if (instruction == "JZ") {
            code.push_back(0x31);
            add_address(get_value(words[1]));
        }
        else if (instruction == "JNZ") {
            code.push_back(0x32);
            add_address(get_value(words[1]));
        }
        else if (instruction == "JC") {
            code.push_back(0x33);
            add_address(get_value(words[1]));
        }
        else if (instruction == "JNC") {
            code.push_back(0x34);
            add_address(get_value(words[1]));
        }

        else if (instruction == "CLS") code.push_back(0x40);
        else if (instruction == "PLOT") code.push_back(0x41);
        else if (instruction == "UNPLOT") code.push_back(0x42);
        else if (instruction == "SHOW") code.push_back(0x43);

        else if (instruction == "PUSH") {
            if (words[1] == "A") code.push_back(0x50);
            else if (words[1] == "B") code.push_back(0x52);
            else if (words[1] == "H") code.push_back(0x56);
            else throw runtime_error("Invalid PUSH register");
        }

        else if (instruction == "POP") {
            if (words[1] == "A") code.push_back(0x51);
            else if (words[1] == "B") code.push_back(0x53);
            else if (words[1] == "H") code.push_back(0x57);
            else throw runtime_error("Invalid POP register");
        }

        else if (instruction == "CALL") {
            code.push_back(0x54);
            add_address(get_value(words[1]));
        }

        else if (instruction == "RET") code.push_back(0x55);
        else if (instruction == "ALLOC") code.push_back(0x60);

        else
            throw runtime_error("Unknown instruction: " + instruction);
    }
}

vector<unsigned char> Assembler::assemble() {
    first_pass();
    second_pass();

    return code;
}
