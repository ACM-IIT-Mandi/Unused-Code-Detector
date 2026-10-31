#ifndef LEXER_HPP
#define LEXER_HPP

#include <string>
#include <vector>

enum class TokenKind {
    Identifier,
    Keyword,
    Number,
    String,
    Operator,
    Punctuation,
    Newline,
    Indent,
    Dedent,
    EndOfFile,
    Error
};

struct Token {
    TokenKind kind;
    std::string lexeme;
    int line;
    int column;
};

std::vector<Token> tokenize(const std::string& source);

#endif