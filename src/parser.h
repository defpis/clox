#ifndef LOX_PARSER_H
#define LOX_PARSER_H

#include "expr.h"
#include <vector>

class ParserError : public std::exception {
public:
  std::string message;
  explicit ParserError(std::string message) : message(std::move(message)) {}
};

class Parser {
private:
  std::vector<SPToken> _tokens;
  int _current;

  void reset();

  bool isAtEnd();
  bool match(const std::vector<TokenType> &types);
  bool check(TokenType type);

  SPToken advance();
  SPToken peek();
  SPToken peekPrev();
  SPToken consume(TokenType type, const std::string &message);

  SPExpr expression();
  SPExpr equality();
  SPExpr comparison();
  SPExpr term();
  SPExpr factor();
  SPExpr unary();
  SPExpr primary();

  static ParserError error(const SPToken &token, const std::string &message);

  void synchronize();

public:
  std::optional<SPExpr> parse(const std::vector<SPToken> &tokens);
};

#endif // LOX_PARSER_H
