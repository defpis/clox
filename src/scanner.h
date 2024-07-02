#ifndef CLOX_SCANNER_H
#define CLOX_SCANNER_H

#include "token.h"
#include <vector>

class Scanner {
private:
  static std::map<std::string, TokenType> keywords;
  static std::optional<TokenType> keywordType(std::string &keyword);

  std::string code;
  std::vector<SPToken> tokens;

  int start = 0;
  int current = 0;
  int line = 0;

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

  Scanner() = default;

public:
  static Scanner &getInstance();
  Scanner(const Scanner &) = delete;
  Scanner &operator=(const Scanner &) = delete;

  std::vector<SPToken> scanTokens(const std::string &_code);
};

#endif // CLOX_SCANNER_H
