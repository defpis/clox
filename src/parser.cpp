#include "parser.h"
#include "lox.h"

void Parser::reset() {
  tokens.clear();
  current = 0;
}

SPToken Parser::advance() {
  if (!isAtEnd()) {
    current++;
  }
  return peekPrev();
}

bool Parser::match(const std::vector<TokenType> &types) {
  for (auto &type : types) { // NOLINT(*-use-anyofallof)
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

bool Parser::checkNext(TokenType type) {
  if (isAtEnd()) {
    return false;
  }
  return peekNext()->type == type;
}

SPToken Parser::peek() { return tokens.at(current); }

SPToken Parser::peekPrev() { return tokens.at(current - 1); }

SPToken Parser::peekNext() { return tokens.at(current + 1); }

bool Parser::isAtEnd() { return peek()->type == TokenType::EOF_; }

SPExpr Parser::expression() { return assignment(); } // NOLINT(*-no-recursion)

SPExpr Parser::assignment() { // NOLINT(*-no-recursion)
  SPExpr expr = or_();

  if (match({TokenType::EQUAL, TokenType::MINUS_EQUAL, TokenType::PLUS_EQUAL, TokenType::SLASH_EQUAL,
             TokenType::STAR_EQUAL})) {
    SPToken op = peekPrev();
    SPExpr value = assignment();

    if (auto p = std::dynamic_pointer_cast<VariableExpr>(expr)) {
      // a = 1 => by default

      // a -= 1 => a = a - 1
      if (op->type == TokenType::MINUS_EQUAL) {
        SPToken newOp = std::make_shared<Token>(TokenType::MINUS, "-", nullptr, op->line);
        value = std::make_shared<BinaryExpr>(expr, newOp, value);
      }
      // a += 1 => a = a + 1
      if (op->type == TokenType::PLUS_EQUAL) {
        SPToken newOp = std::make_shared<Token>(TokenType::PLUS, "+", nullptr, op->line);
        value = std::make_shared<BinaryExpr>(expr, newOp, value);
      }
      // a /= 1 => a = a / 1
      if (op->type == TokenType::SLASH_EQUAL) {
        SPToken newOp = std::make_shared<Token>(TokenType::SLASH, "/", nullptr, op->line);
        value = std::make_shared<BinaryExpr>(expr, newOp, value);
      }
      // a *= 1 => a = a * 1
      if (op->type == TokenType::STAR_EQUAL) {
        SPToken newOp = std::make_shared<Token>(TokenType::STAR, "*", nullptr, op->line);
        value = std::make_shared<BinaryExpr>(expr, newOp, value);
      }

      return std::make_shared<AssignExpr>(p->name, value, false);
    } else if (auto p = std::dynamic_pointer_cast<GetExpr>(expr)) {
      return std::make_shared<SetExpr>(p->object, p->name, value);
    }

    throw error(op, "Invalid assignment target.");
  }

  return expr;
}

SPExpr Parser::equality() { // NOLINT(*-no-recursion)
  SPExpr expr = comparison();

  while (match({TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL})) {
    SPToken op = peekPrev();
    SPExpr right = comparison();
    expr = std::make_shared<BinaryExpr>(expr, op, right);
  }

  return expr;
}

SPExpr Parser::comparison() { // NOLINT(*-no-recursion)
  SPExpr expr = term();

  while (match({TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL})) {
    SPToken op = peekPrev();
    SPExpr right = term();
    expr = std::make_shared<BinaryExpr>(expr, op, right);
  }

  return expr;
}

SPExpr Parser::term() { // NOLINT(*-no-recursion)
  SPExpr expr = factor();

  while (match({TokenType::MINUS, TokenType::PLUS})) {
    SPToken op = peekPrev();
    SPExpr right = factor();
    expr = std::make_shared<BinaryExpr>(expr, op, right);
  }

  return expr;
}

SPExpr Parser::factor() { // NOLINT(*-no-recursion)
  SPExpr expr = exp();

  while (match({TokenType::SLASH, TokenType::STAR})) {
    SPToken op = peekPrev();
    SPExpr right = exp();
    expr = std::make_shared<BinaryExpr>(expr, op, right);
  }

  return expr;
}

SPExpr Parser::exp() { // NOLINT(*-no-recursion)
  SPExpr expr = unary();

  while (match({TokenType::STAR_STAR})) {
    SPToken op = peekPrev();
    SPExpr right = unary();
    expr = std::make_shared<BinaryExpr>(expr, op, right);
  }

  return expr;
}

std::shared_ptr<AssignExpr> Parser::selfOperatingUnaryToAssign(SPToken op, std::shared_ptr<VariableExpr> var,
                                                               bool returnOriginal) {
  SPExpr one = std::make_shared<LiteralExpr>(1);

  SPToken newOp = nullptr;
  if (op->type == TokenType::MINUS_MINUS) {
    newOp = std::make_shared<Token>(TokenType::MINUS, "-", nullptr, op->line);
  } else {
    newOp = std::make_shared<Token>(TokenType::PLUS, "+", nullptr, op->line);
  }

  SPExpr value = std::make_shared<BinaryExpr>(var, newOp, one);
  return std::make_shared<AssignExpr>(var->name, value, returnOriginal);
}

SPExpr Parser::unary() { // NOLINT(*-no-recursion)
  if (match({TokenType::BANG})) {
    SPToken op = peekPrev();
    SPExpr right = unary();
    return std::make_shared<UnaryExpr>(op, right);
  }

  // --a|++a => a=a-1|a=a+1
  if (match({TokenType::MINUS_MINUS, TokenType::PLUS_PLUS})) {
    SPToken op = peekPrev();
    SPExpr expr = call();
    if (auto p = std::dynamic_pointer_cast<VariableExpr>(expr)) {
      return selfOperatingUnaryToAssign(op, p, false);
    }
    throw error(op, "Expect variable after '" + op->lexeme + "'.");
  }

  // a--|a++ => a=a-1|a=a+1
  if (checkNext(TokenType::MINUS_MINUS) || checkNext(TokenType::PLUS_PLUS)) {
    SPExpr expr = call();
    advance();
    SPToken op = peekPrev();
    if (auto p = std::dynamic_pointer_cast<VariableExpr>(expr)) {
      return selfOperatingUnaryToAssign(op, p, true);
    }
    throw error(op, "Expect variable before '" + op->lexeme + "'.");
  }

  if (match({TokenType::PLUS, TokenType::MINUS})) {
    SPToken op = peekPrev();
    SPExpr right = call();
    return std::make_shared<UnaryExpr>(op, right);
  }

  return call();
}

SPExpr Parser::call() { // NOLINT(*-no-recursion)
  SPExpr expr = primary();

  while (match({TokenType::LEFT_PAREN})) {
    expr = finishCall(expr);
  }

  while (true) {
    if (match({TokenType::LEFT_PAREN})) {
      expr = finishCall(expr);
    } else if (match({TokenType::DOT})) {
      SPToken name = consume(TokenType::IDENTIFIER, "Expect property name after '.'.");
      expr = std::make_shared<GetExpr>(expr, name);
    } else {
      break;
    }
  }

  return expr;
}

SPExpr Parser::finishCall(SPExpr callee) { // NOLINT(*-no-recursion)
  std::vector<SPExpr> arguments;

  if (!check(TokenType::RIGHT_PAREN)) {
    do {
      if (arguments.size() >= 255) {
        throw error(peek(), "Can't have more than 255 arguments.");
      }
      arguments.push_back(expression());
    } while (match({TokenType::COMMA}));
  }

  SPToken paren = consume(TokenType::RIGHT_PAREN, "Expect ')' after arguments.");

  return std::make_shared<CallExpr>(callee, paren, arguments);
}

SPExpr Parser::primary() { // NOLINT(*-no-recursion)
  if (match({TokenType::THIS})) {
    return std::make_shared<ThisExpr>(peekPrev());
  }
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
  if (match({TokenType::IDENTIFIER})) {
    return std::make_shared<VariableExpr>(peekPrev());
  }
  if (match({TokenType::LEFT_PAREN})) {
    SPExpr expr = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
    return std::make_shared<GroupingExpr>(expr);
  }
  throw error(peek(), "Expect expression.");
}

SPExpr Parser::or_() { // NOLINT(*-no-recursion)
  SPExpr expr = and_();

  while (match({TokenType::OR})) {
    SPToken op = peekPrev();
    SPExpr right = and_();
    expr = std::make_shared<LogicalExpr>(expr, op, right);
  }

  return expr;
}

SPExpr Parser::and_() { // NOLINT(*-no-recursion)
  SPExpr expr = equality();

  while (match({TokenType::AND})) {
    SPToken op = peekPrev();
    SPExpr right = equality();
    expr = std::make_shared<LogicalExpr>(expr, op, right);
  }

  return expr;
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

SPStmt Parser::statement() { // NOLINT(*-no-recursion)
  if (match({TokenType::FOR})) {
    return forStatement();
  }

  if (match({TokenType::WHILE})) {
    return whileStatement();
  }

  if (match({TokenType::IF})) {
    return ifStatement();
  }

  if (match({TokenType::RETURN})) {
    return returnStatement();
  }

  if (match({TokenType::PRINT})) {
    return printStatement();
  }

  if (match({TokenType::LEFT_BRACE})) {
    return blockStatement();
  }

  return exprStatement();
}

SPStmt Parser::returnStatement() {
  SPToken keyword = peekPrev();
  SPExpr value = nullptr;
  if (!check(TokenType::SEMICOLON)) {
    value = expression();
  }
  consume(TokenType::SEMICOLON, "Expect ';' after return value.");
  return std::make_shared<ReturnStmt>(keyword, value);
}

SPStmt Parser::declaration() { // NOLINT(*-no-recursion)
  try {
    if (match({TokenType::CLASS})) {
      return classDeclaration();
    }
    if (match({TokenType::FUN})) {
      return funDeclaration("function");
    }
    if (match({TokenType::VAR})) {
      return varDeclaration();
    }
    return statement();
  } catch (ParseError &err) {
    synchronize();
    return nullptr;
  }
}

std::shared_ptr<FunStmt> Parser::funDeclaration(const std::string &kind) { // NOLINT(*-no-recursion)
  SPToken name = consume(TokenType::IDENTIFIER, "Expect " + kind + " name.");
  consume(TokenType::LEFT_PAREN, "Expect '(' after " + kind + " name.");
  std::vector<SPToken> parameters;
  if (!check(TokenType::RIGHT_PAREN)) {
    do {
      if (parameters.size() >= 255) {
        throw error(peek(), "Can't have more than 255 parameters.");
      }
      parameters.push_back(consume(TokenType::IDENTIFIER, "Expect parameter name."));
    } while (match({TokenType::COMMA}));
  }
  consume(TokenType::RIGHT_PAREN, "Expect ')' after parameters.");

  consume(TokenType::LEFT_BRACE, "Expect '{' before " + kind + " body.");
  std::shared_ptr<BlockStmt> body = blockStatement();
  return std::make_shared<FunStmt>(name, parameters, body);
}

SPStmt Parser::classDeclaration() { // NOLINT(*-no-recursion)
  SPToken name = consume(TokenType::IDENTIFIER, "Expect class name.");
  consume(TokenType::LEFT_BRACE, "Expect '{' before class body.");

  std::vector<std::shared_ptr<VarStmt>> attributes;
  std::vector<std::shared_ptr<FunStmt>> methods;

  while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
    tryParse = true;
    auto start = current;

    try {
      auto attribute = varDeclaration();
      attributes.push_back(attribute);
    } catch (ParseError &err) {
      tryParse = false;
      current = start; // 重新设置索引已解析

      auto method = funDeclaration("method");
      methods.push_back(method);
    }
  }

  consume(TokenType::RIGHT_BRACE, "Expect '}' after class body.");

  return std::make_shared<ClassStmt>(name, attributes, methods);
}

std::shared_ptr<VarStmt> Parser::varDeclaration() {
  SPToken name = consume(TokenType::IDENTIFIER, "Expect variable name.");

  SPExpr initializer;
  if (match({TokenType::EQUAL})) {
    initializer = expression();
  }

  consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
  return std::make_shared<VarStmt>(name, initializer);
}

SPStmt Parser::exprStatement() {
  SPExpr expr = expression();
  consume(TokenType::SEMICOLON, "Expect ';' after expression.");
  return std::make_shared<ExprStmt>(expr);
}

SPStmt Parser::printStatement() {
  SPExpr expr = expression();
  consume(TokenType::SEMICOLON, "Expect ';' after expression.");
  return std::make_shared<PrintStmt>(expr);
}

std::shared_ptr<BlockStmt> Parser::blockStatement() { // NOLINT(*-no-recursion)
  std::vector<SPStmt> statements;

  while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
    statements.push_back(declaration());
  }

  consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
  return std::make_shared<BlockStmt>(statements);
}

SPStmt Parser::ifStatement() { // NOLINT(*-no-recursion)
  consume(TokenType::LEFT_PAREN, "Expect '(' after 'if'.");
  SPExpr condition = expression();
  consume(TokenType::RIGHT_PAREN, "Expect ')' after if condition.");

  SPStmt thenBranch = statement();
  SPStmt elseBranch;
  if (match({TokenType::ELSE})) {
    elseBranch = statement();
  }

  return std::make_shared<IfStmt>(condition, thenBranch, elseBranch);
}

SPStmt Parser::whileStatement() { // NOLINT(*-no-recursion)
  consume(TokenType::LEFT_PAREN, "Expect '(' after 'while'.");
  SPExpr condition = expression();
  consume(TokenType::RIGHT_PAREN, "Expect ')' after while condition.");
  SPStmt body = statement();

  return std::make_shared<WhileStmt>(condition, body);
}

SPStmt Parser::forStatement() { // NOLINT(*-no-recursion)
  consume(TokenType::LEFT_PAREN, "Expect '(' after 'for'.");

  SPStmt initializer = nullptr;
  if (match({TokenType::SEMICOLON})) {
    // for (; ...; ...)
  } else if (match({TokenType::VAR})) {
    initializer = varDeclaration(); // for (var a = 0; ...; ...)
  } else {
    initializer = exprStatement(); // for (expression; ...; ...)
  }

  SPExpr condition;
  if (!check(TokenType::SEMICOLON)) {
    condition = expression(); // for (...; condition; ...)
  }
  consume(TokenType::SEMICOLON, "Expect ';' after loop condition.");

  SPExpr increment;
  if (!check(TokenType::RIGHT_PAREN)) {
    increment = expression(); // for (...; ...; increment)
  }
  consume(TokenType::RIGHT_PAREN, "Expect ')' after for clauses.");

  SPStmt body = statement();

  if (increment) {
    body = std::make_shared<BlockStmt>(std::vector<SPStmt>{body, std::make_shared<ExprStmt>(increment)});
  }

  if (!condition) {
    condition = std::make_shared<LiteralExpr>(true);
  }

  body = std::make_shared<WhileStmt>(condition, body);

  if (initializer) {
    body = std::make_shared<BlockStmt>(std::vector<SPStmt>{initializer, body});
  }

  return body;
}

bool Parser::tryParse = false;

ParseError Parser::error(SPToken token, const std::string &message) {
  if (!tryParse) {
    lox::error(std::move(token), message);
  }
  return {};
}

std::vector<SPStmt> Parser::parse(std::vector<SPToken> &_tokens) {
  reset();

  tokens = _tokens;

  std::vector<SPStmt> statements;

  while (!isAtEnd()) {
    // 存在空指针，可能有问题
    SPStmt statement = declaration();
    if (statement) {
      statements.push_back(statement);
    }
  }

  return statements;
}

Parser &Parser::getInstance() {
  static Parser instance;
  return instance;
}
