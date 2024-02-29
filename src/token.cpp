#include "token.h"
#include "util.h"

std::map<TokenType, std::string> Token::types = {
    // extra keywords
    {TokenType::GETTER, "GETTER"},
    {TokenType::SETTER, "SETTER"},
    {TokenType::STATIC, "STATIC"},

    // extra operators
    {TokenType::MINUS_MINUS, "MINUS_MINUS"},
    {TokenType::PLUS_PLUS, "PLUS_PLUS"},
    {TokenType::MINUS_EQUAL, "MINUS_EQUAL"},
    {TokenType::PLUS_EQUAL, "PLUS_EQUAL"},
    {TokenType::SLASH_EQUAL, "SLASH_EQUAL"},
    {TokenType::STAR_EQUAL, "STAR_EQUAL"},
    {TokenType::STAR_STAR, "STAR_STAR"},

    // Single character tokens.
    {TokenType::LEFT_PAREN, "LEFT_PAREN"},
    {TokenType::RIGHT_PAREN, "RIGHT_PAREN"},
    {TokenType::LEFT_BRACE, "LEFT_BRACE"},
    {TokenType::RIGHT_BRACE, "RIGHT_BRACE"},
    {TokenType::COMMA, "COMMA"},
    {TokenType::DOT, "DOT"},
    {TokenType::MINUS, "MINUS"},
    {TokenType::PLUS, "PLUS"},
    {TokenType::SEMICOLON, "SEMICOLON"},
    {TokenType::SLASH, "SLASH"},
    {TokenType::STAR, "STAR"},

    // One or two character tokens.
    {TokenType::BANG, "BANG"},
    {TokenType::BANG_EQUAL, "BANG_EQUAL"},
    {TokenType::EQUAL, "EQUAL"},
    {TokenType::EQUAL_EQUAL, "EQUAL_EQUAL"},
    {TokenType::GREATER, "GREATER"},
    {TokenType::GREATER_EQUAL, "GREATER_EQUAL"},
    {TokenType::LESS, "LESS"},
    {TokenType::LESS_EQUAL, "LESS_EQUAL"},

    // Literals.
    {TokenType::IDENTIFIER, "IDENTIFIER"},
    {TokenType::STRING, "STRING"},
    {TokenType::NUMBER, "NUMBER"},

    // Keywords.
    {TokenType::AND, "AND"},
    {TokenType::CLASS, "CLASS"},
    {TokenType::ELSE, "ELSE"},
    {TokenType::FALSE, "FALSE"},
    {TokenType::FUN, "FUN"},
    {TokenType::FOR, "FOR"},
    {TokenType::IF, "IF"},
    {TokenType::NIL, "NIL"},
    {TokenType::OR, "OR"},
    {TokenType::PRINT, "PRINT"},
    {TokenType::RETURN, "RETURN"},
    {TokenType::SUPER, "SUPER"},
    {TokenType::THIS, "THIS"},
    {TokenType::TRUE, "TRUE"},
    {TokenType::VAR, "VAR"},
    {TokenType::WHILE, "WHILE"},

    {TokenType::EOF_, "EOF"},
};

std::optional<std::string> Token::typeString(TokenType type) {
  auto it = types.find(type);
  if (it != types.end()) {
    return it->second;
  }
  return std::nullopt;
}

std::string Token::toString() {
  std::optional<std::string> opt = typeString(type);
  std::string typeStr = opt.has_value() ? opt.value() : "UNKNOWN";
  return "Token<" +
         util::joinString({typeStr, lexeme, util::toString(literal, "object"), util::toString(line, "-1")}, " | ") +
         ">";
}
