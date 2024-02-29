#include "parser.h"
#include "lox.h"

void Parser::reset() {
  _tokens.clear();
  _current = 0;
}

SPToken Parser::advance() {
  if (!isAtEnd()) {
    _current++;
  }
  return peekPrev();
}

bool Parser::match(const std::vector<TokenType> &types) {
  for (auto &type : types) {
    if (check(type)) {
      advance();
      return true;
    }
  }
  return false;
}

SPToken Parser::consume(TokenType type, const std::string &message) {
  if (check(type)) {
    return advance();
  }
  throw error(peek(), message);
}

bool Parser::check(TokenType type) {
  if (isAtEnd()) {
    return false;
  }
  return peek()->type == type;
}

SPToken Parser::peek() { return _tokens.at(_current); }

SPToken Parser::peekPrev() { return _tokens.at(_current - 1); }

bool Parser::isAtEnd() { return peek()->type == TokenType::EOF_; }

SPExpr Parser::expression() { return equality(); }

SPExpr Parser::equality() {
  SPExpr expr = comparison();

  while (match({TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL})) {
    SPToken op = peekPrev();
    SPExpr right = comparison();
    expr = std::make_shared<BinaryExpr>(expr, op, right);
  }

  return expr;
}

SPExpr Parser::comparison() {
  SPExpr expr = term();

  while (match({TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL})) {
    SPToken op = peekPrev();
    SPExpr right = term();
    expr = std::make_shared<BinaryExpr>(expr, op, right);
  }

  return expr;
}

SPExpr Parser::term() {
  SPExpr expr = factor();

  while (match({TokenType::MINUS, TokenType::PLUS})) {
    SPToken op = peekPrev();
    SPExpr right = factor();
    expr = std::make_shared<BinaryExpr>(expr, op, right);
  }

  return expr;
}

SPExpr Parser::factor() {
  SPExpr expr = unary();

  while (match({TokenType::SLASH, TokenType::STAR})) {
    SPToken op = peekPrev();
    SPExpr right = unary();
    expr = std::make_shared<BinaryExpr>(expr, op, right);
  }

  return expr;
}

SPExpr Parser::unary() {
  if (match({TokenType::BANG, TokenType::MINUS})) {
    SPToken op = peekPrev();
    SPExpr right = unary();
    return std::make_shared<UnaryExpr>(op, right);
  }

  return primary();
}

SPExpr Parser::primary() {
  if (match({TokenType::FALSE})) {
    return std::make_shared<LiteralExpr>(false);
  }
  if (match({TokenType::TRUE})) {
    return std::make_shared<LiteralExpr>(true);
  }
  if (match({TokenType::NIL})) {
    return std::make_shared<LiteralExpr>(nullptr);
  }
  if (match({TokenType::STRING, TokenType::NUMBER})) {
    return std::make_shared<LiteralExpr>(peekPrev()->literal);
  }
  if (match({TokenType::LEFT_PAREN})) {
    SPExpr expr = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
    return std::make_shared<GroupingExpr>(expr);
  }
  throw error(peek(), "Expect expression.");
}

ParserError Parser::error(const SPToken &token, const std::string &message) {
  Lox::error(token, message);
  return ParserError(message);
}

void Parser::synchronize() {
  advance();

  while (!isAtEnd()) {
    if (peekPrev()->type == TokenType::SEMICOLON) {
      return;
    }

    switch (peek()->type) {
      case TokenType::CLASS:
      case TokenType::FUN:
      case TokenType::VAR:
      case TokenType::FOR:
      case TokenType::IF:
      case TokenType::WHILE:
      case TokenType::PRINT:
      case TokenType::RETURN: {
        return;
      }
      default: {
      }
    }

    advance();
  }
}

std::optional<SPExpr> Parser::parse(const std::vector<SPToken> &tokens) {
  reset();

  _tokens = tokens;

  try {
    return expression();
  } catch (ParserError &err) {
    return std::nullopt;
  }
}
