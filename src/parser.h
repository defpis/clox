#ifndef CLOX_PARSER_H
#define CLOX_PARSER_H

#include "expr.h"
#include "stmt.h"
#include <vector>

class ParseError : public std::exception {};

class Parser {
private:
  std::vector<SPToken> tokens;
  int current = 0;

  void reset();

  bool isAtEnd();
  bool match(const std::vector<TokenType> &types);
  bool check(TokenType type);
  bool checkNext(TokenType type);

  SPToken advance();
  SPToken peek();
  SPToken peekPrev();
  SPToken peekNext();
  SPToken consume(TokenType type, const std::string &message);

  SPExpr expression();
  SPExpr assignment();
  SPExpr equality();
  SPExpr comparison();
  SPExpr term();
  SPExpr factor();
  SPExpr exp();
  SPExpr unary();
  SPExpr call();
  SPExpr finishCall(SPExpr expr);
  SPExpr primary();
  SPExpr or_();
  SPExpr and_();

  void synchronize();

  SPStmt statement();

  SPStmt declaration();
  std::shared_ptr<FunStmt> funDeclaration(const std::string &kind);
  SPStmt classDeclaration();
  std::shared_ptr<VarStmt> varDeclaration();

  SPStmt exprStatement();
  SPStmt returnStatement();
  SPStmt printStatement();
  std::shared_ptr<BlockStmt> blockStatement();
  SPStmt ifStatement();
  SPStmt whileStatement();
  SPStmt forStatement();

  Parser() = default;

public:
  static Parser &getInstance();
  Parser(const Parser &) = delete;
  Parser &operator=(const Parser &) = delete;

  static ParseError error(SPToken token, const std::string &message);
  std::vector<SPStmt> parse(std::vector<SPToken> &_tokens);
};

#endif // CLOX_PARSER_H
