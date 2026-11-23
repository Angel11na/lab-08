
# М1 — Lexer

## 1. Структура токенів

**Файл:** `src/asm/token.hpp`

Створено типи токенів та структуру для зберігання їхнього значення і номера рядка.

```cpp
enum class TokenType {
    Identifier,
    Number,
    Comma,
    Colon,
    NewLine,
    EndOfFile
};

struct Token {
    TokenType type;
    string text;
    int line;
};
```

## 2. Оголошення лексера

**Файл:** `src/asm/lexer.hpp`

Створено клас `Lexer`, який зберігає вхідний текст, поточну позицію та номер рядка.

```cpp
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
```

## 3. Реалізація лексера

**Файл:** `src/asm/lexer.cpp`

### Розпізнавання ідентифікаторів

Метод `identifier()` зчитує назви команд та інші ідентифікатори.

```cpp
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
```

### Розпізнавання чисел

Метод `number()` підтримує десяткові, шістнадцяткові та двійкові числа, а також перевіряє їхній правильний запис.

```cpp
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
```

### Основний метод tokenize()

Метод `tokenize()` проходить текст програми та формує список токенів

```cpp
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
```

## 4. Тестування
``
~/Documents/progamming/lab-08
❯ ./build/lexer_test
Line: 1 | Type: 0 | Text: LOADI_A
Line: 1 | Type: 1 | Text: 25
Line: 1 | Type: 4 | Text: 

Line: 2 | Type: 0 | Text: LOADI_B
Line: 2 | Type: 1 | Text: 0x1A
Line: 2 | Type: 4 | Text: 

Line: 3 | Type: 0 | Text: LOADI_A
Line: 3 | Type: 1 | Text: 0b1010
Line: 3 | Type: 4 | Text: 

Line: 4 | Type: 5 | Text: 

~/Documents/progamming/lab-08
❯ 



# М2 — Assembler

## 1. Token

Файл: [`token.hpp`](src/asm/token.hpp)

Описує типи токенів та їхню структуру.

## 2. Lexer

Файли: [`lexer.hpp`](src/asm/lexer.hpp), [`lexer.cpp`](src/asm/lexer.cpp)

Перетворює текст програми на токени. Розпізнає команди, числа, мітки, розділові знаки та коментарі.

Основні функції:
- `peek()` — перегляд поточного символу.
- `advance()` — перехід до наступного символу.
- `tokenize()` — створення списку токенів.
- `identifier()` — розпізнавання ідентифікаторів.
- `number()` — розпізнавання чисел.

## 3. Assembler

Файли: [`assembler.hpp`](src/asm/assembler.hpp), [`assembler.cpp`](src/asm/assembler.cpp)

Перетворює токени на машинний код.

Основні функції:
- `parse_number()` — обробка чисел.
- `first_pass()` — визначення адрес міток.
- `second_pass()` — формування машинного коду.
- `assemble()` — запуск асемблювання.

## 4. Команда asm

Файл: [`main.cpp`](src/main.cpp)

Додає команду `asm`, яка зчитує файл, запускає лексер та асемблер і виводить отримані байти.

## 5. Перевірка роботи

Файл: [`test.asm`](src/programs/test.asm)

```asm
; Test HALT instruction
HALT

; Test JMP instruction
JMP 0x0123
```

Команда запуску:

```bash
asm src/programs/test.asm
```

Результат:

```text
assembled bytes:
00 30 23 01
```

`HALT` → `00`, `JMP 0x0123`