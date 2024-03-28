#ifndef CLOX_SCANNER_H
#define CLOX_SCANNER_H

#include "token.h"
#include <vector>

class Scanner {
private:
  static std::map<std::string, TokenType> _keywords;
  static std::optional<TokenType> keywordType(std::string &keyword);

  std::string _code;
  std::vector<SPToken> _tokens;
  int _start;
  int _current;
  int _line;

  void reset();

  char advance();
  char peek();
  char peekPrev();
  char peekNext();

  bool isAtEnd();
  bool match(char c);

  static bool isDigit(char c);
  static bool isAlpha(char c);
  static bool isAlphaNumeric(char c);

  void string(char c);
  void number();
  void identifier();

  void addToken(TokenType type);
  void addToken(TokenType type, const std::any &literal);

  void scanToken();

public:
  std::vector<SPToken> scanTokens(const std::string &code);
};

#endif // CLOX_SCANNER_H
