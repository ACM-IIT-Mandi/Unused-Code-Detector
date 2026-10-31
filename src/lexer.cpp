#include "lexer.hpp"

bool isIdentifierStart(char c) {
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
           c == '_';
}

bool isIdentifierContinue(char c) {
    return isIdentifierStart(c) ||
           (c >= '0' && c <= '9');
}

#include "lexer.hpp"

#include <cctype>
#include <unordered_set>
#include <string>
#include <vector>

bool isIdentifierStart(char c) {
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
           c == '_';
}

bool isIdentifierContinue(char c) {
    return isIdentifierStart(c) ||
           (c >= '0' && c <= '9');
}

bool isKeyword(const std::string& word) {
    static const std::unordered_set<std::string> keywords = {
        "if", "else", "elif", "while", "for",
        "def", "return", "class", "import", "from",
        "as", "pass", "break", "continue", "in",
        "is", "not", "and", "or", "True", "False",
        "None", "try", "except", "finally", "raise",
        "with", "lambda", "global", "nonlocal",
        "assert", "del", "yield", "async", "await"
    };

    return keywords.find(word) != keywords.end();
}

std::vector<Token> tokenize(const std::string& source) {
    std::vector<Token> tokens;

    std::size_t pos = 0;
    int line = 1;
    int column = 1;

    while (pos < source.length()) {
        char c = source[pos];

        // 1. Ignore ordinary spaces and tabs.
        if (c == ' ' || c == '\t') {
            ++pos;
            ++column;
            continue;
        }

        // 2. Handle newlines.
        if (c == '\n') {
            tokens.push_back({
                TokenKind::Newline, "\\n", line, column
            });

            ++pos;
            ++line;
            column = 1;
            continue;
        }

        // Handle Windows-style CRLF line endings.
        if (c == '\r') {
            ++pos;

            if (pos < source.length() && source[pos] == '\n') {
                ++pos;
            }

            tokens.push_back({
                TokenKind::Newline, "\\n", line, column
            });

            ++line;
            column = 1;
            continue;
        }

        // 3. Skip comments.
        if (c == '#') {
            while (pos < source.length() &&
                   source[pos] != '\n' &&
                   source[pos] != '\r') {
                ++pos;
                ++column;
            }

            continue;
        }

        // 4. Recognize identifiers and keywords.
        if (isIdentifierStart(c)) {
            std::size_t start = pos;
            int startColumn = column;

            ++pos;
            ++column;

            while (pos < source.length() &&
                   isIdentifierContinue(source[pos])) {
                ++pos;
                ++column;
            }

            std::string word = source.substr(start, pos - start);

            TokenKind kind = isKeyword(word)
                ? TokenKind::Keyword
                : TokenKind::Identifier;

            tokens.push_back({
                kind, word, line, startColumn
            });

            continue;
        }

        // 5. Recognize integer literals.
        if (std::isdigit(static_cast<unsigned char>(c))) {
            std::size_t start = pos;
            int startColumn = column;

            while (pos < source.length() &&
                   std::isdigit(
                       static_cast<unsigned char>(source[pos]))) {
                ++pos;
                ++column;
            }

            tokens.push_back({
                TokenKind::Number,
                source.substr(start, pos - start),
                line,
                startColumn
            });

            continue;
        }

        // 6. Recognize operators.
        if (c == '+' || c == '-' || c == '*' ||
            c == '/' || c == '%' || c == '=' ||
            c == '<' || c == '>' || c == '!') {

            std::size_t start = pos;
            int startColumn = column;

            ++pos;
            ++column;

            // Recognize common two-character operators.
            if (pos < source.length()) {
                std::string twoChar =
                    source.substr(start, 2);

                if (twoChar == "==" || twoChar == "!=" ||
                    twoChar == "<=" || twoChar == ">=" ||
                    twoChar == "**" || twoChar == "//" ||
                    twoChar == "+=" || twoChar == "-=" ||
                    twoChar == "*=" || twoChar == "/=" ||
                    twoChar == "%=") {
                    ++pos;
                    ++column;
                }
            }

            tokens.push_back({
                TokenKind::Operator,
                source.substr(start, pos - start),
                line,
                startColumn
            });

            continue;
        }

        // 7. Recognize punctuation.
        if (c == '(' || c == ')' ||
            c == '[' || c == ']' ||
            c == '{' || c == '}' ||
            c == ',' || c == ':' ||
            c == '.' || c == ';') {

            tokens.push_back({
                TokenKind::Punctuation,
                std::string(1, c),
                line,
                column
            });

            ++pos;
            ++column;
            continue;
        }

        // 8. Unknown character: report an error token.
        tokens.push_back({
            TokenKind::Error,
            std::string(1, c),
            line,
            column
        });

        ++pos;
        ++column;
    }

    tokens.push_back({
        TokenKind::EndOfFile, "", line, column
    });

    return tokens;
}
