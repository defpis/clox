#ifndef CLOX_TOKEN_H
#define CLOX_TOKEN_H

#include <any>
#include <map>
#include <string>

class Token {
public:
  enum class Type {
    GETTER, // getter
    SETTER, // setter
    STATIC, // static

    MINUS_MINUS, // --
    PLUS_PLUS,   // ++
    MINUS_EQUAL, // -=
    PLUS_EQUAL,  // +=
    SLASH_EQUAL, // /=
    STAR_EQUAL,  // *=
    STAR_STAR,   // **

    // Single character tokens.
    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACE,
    RIGHT_BRACE,
    COMMA,
    DOT,
    MINUS,
    PLUS,
    SEMICOLON,
    SLASH,
    STAR,

    // One or two character tokens.
    BANG,
    BANG_EQUAL,
    EQUAL,
    EQUAL_EQUAL,
    GREATER,
    GREATER_EQUAL,
    LESS,
    LESS_EQUAL,

    // Literals.
    IDENTIFIER,
    STRING,
    NUMBER,

    // Keywords.
    AND,
    CLASS,
    ELSE,
    FALSE,
    FUN,
    FOR,
    IF,
    NIL,
    OR,
    PRINT,
    RETURN,
    SUPER,
    THIS,
    TRUE,
    VAR,
    WHILE,

    EOF_,
  };

  static std::map<Type, std::string> types;
  static std::optional<std::string> typeString(Type type);

  Type type;
  std::string lexeme;
  std::any literal;
  int line;

  Token(Type type, std::string lexeme, std::any literal, int line)
      : type(type), lexeme(std::move(lexeme)), literal(std::move(literal)), line(line) {}

  std::string toString();
};

using TokenType = Token::Type;
using SPToken = std::shared_ptr<Token>;

#endif // CLOX_TOKEN_H
