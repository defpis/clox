#ifndef CLOX_EXPR_H
#define CLOX_EXPR_H

#include "token.h"

class Expr {
public:
  virtual ~Expr() = default;
};

using SPExpr = std::shared_ptr<Expr>;

class BinaryExpr : public Expr {
public:
  SPExpr left;
  SPToken op;
  SPExpr right;

  ~BinaryExpr() override = default;

  BinaryExpr(SPExpr left, SPToken op, SPExpr right)
      : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}
};

class GroupingExpr : public Expr {
public:
  SPExpr expression;

  ~GroupingExpr() override = default;

  explicit GroupingExpr(SPExpr expression) : expression(std::move(expression)) {}
};

class UnaryExpr : public Expr {
public:
  SPToken op;
  SPExpr right;

  ~UnaryExpr() override = default;

  UnaryExpr(SPToken op, SPExpr right) : op(std::move(op)), right(std::move(right)) {}
};

class LiteralExpr : public Expr {
public:
  std::any value;

  ~LiteralExpr() override = default;

  explicit LiteralExpr(std::any value) : value(std::move(value)) {}
};

class VariableExpr : public Expr {
public:
  SPToken name;

  ~VariableExpr() override = default;

  explicit VariableExpr(SPToken name) : name(std::move(name)) {}
};

class AssignExpr : public Expr {
public:
  SPToken name;
  SPExpr value;
  bool returnOriginal;

  ~AssignExpr() override = default;

  AssignExpr(SPToken name, SPExpr value, bool returnOriginal)
      : name(std::move(name)), value(std::move(value)), returnOriginal(returnOriginal) {}
};

class LogicalExpr : public Expr {
public:
  SPExpr left;
  SPToken op;
  SPExpr right;

  ~LogicalExpr() override = default;

  LogicalExpr(SPExpr left, SPToken op, SPExpr right)
      : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}
};

class CallExpr : public Expr {
public:
  SPExpr callee;
  SPToken paren;
  std::vector<SPExpr> arguments;

  ~CallExpr() override = default;

  CallExpr(SPExpr callee, SPToken paren, std::vector<SPExpr> arguments)
      : callee(std::move(callee)), paren(std::move(paren)), arguments(std::move(arguments)) {}
};

template <typename R> class ExprVisitor {
protected:
  R visitExpr(SPExpr expr) {
    if (auto p = std::dynamic_pointer_cast<BinaryExpr>(expr)) {
      return visitBinaryExpr(p);
    }
    if (auto p = std::dynamic_pointer_cast<GroupingExpr>(expr)) {
      return visitGroupingExpr(p);
    }
    if (auto p = std::dynamic_pointer_cast<UnaryExpr>(expr)) {
      return visitUnaryExpr(p);
    }
    if (auto p = std::dynamic_pointer_cast<LiteralExpr>(expr)) {
      return visitLiteralExpr(p);
    }
    if (auto p = std::dynamic_pointer_cast<VariableExpr>(expr)) {
      return visitVariableExpr(p);
    }
    if (auto p = std::dynamic_pointer_cast<AssignExpr>(expr)) {
      return visitAssignExpr(p);
    }
    if (auto p = std::dynamic_pointer_cast<LogicalExpr>(expr)) {
      return visitLogicalExpr(p);
    }
    if (auto p = std::dynamic_pointer_cast<CallExpr>(expr)) {
      return visitCallExpr(p);
    }
    // Never reach here!
    throw std::runtime_error("Unexpected expression type.");
  }

  virtual R visitBinaryExpr(std::shared_ptr<BinaryExpr> expr) = 0;
  virtual R visitGroupingExpr(std::shared_ptr<GroupingExpr> expr) = 0;
  virtual R visitUnaryExpr(std::shared_ptr<UnaryExpr> expr) = 0;
  virtual R visitLiteralExpr(std::shared_ptr<LiteralExpr> expr) = 0;
  virtual R visitVariableExpr(std::shared_ptr<VariableExpr> expr) = 0;
  virtual R visitAssignExpr(std::shared_ptr<AssignExpr> expr) = 0;
  virtual R visitLogicalExpr(std::shared_ptr<LogicalExpr> expr) = 0;
  virtual R visitCallExpr(std::shared_ptr<CallExpr> expr) = 0;
};

#endif // CLOX_EXPR_H
