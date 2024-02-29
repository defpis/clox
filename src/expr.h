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

class GetExpr : public Expr {
public:
  SPExpr object;
  SPToken name;

  ~GetExpr() override = default;

  GetExpr(SPExpr object, SPToken name) : object(std::move(object)), name(std::move(name)) {}
};

class SetExpr : public Expr {
public:
  SPExpr object;
  SPToken name;
  SPExpr value;
  bool returnOriginal;

  ~SetExpr() override = default;

  SetExpr(SPExpr object, SPToken name, SPExpr value, bool returnOriginal)
      : object(std::move(object)), name(std::move(name)), value(std::move(value)), returnOriginal(returnOriginal) {}
};

class ThisExpr : public Expr {
public:
  SPToken keyword;

  ~ThisExpr() override = default;

  explicit ThisExpr(SPToken keyword) : keyword(std::move(keyword)) {}
};

class SuperExpr : public Expr {
public:
  SPToken keyword;
  SPToken method;

  ~SuperExpr() override = default;

  SuperExpr(SPToken keyword, SPToken method) : keyword(std::move(keyword)), method(std::move(method)) {}
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
    if (auto p = std::dynamic_pointer_cast<GetExpr>(expr)) {
      return visitGetExpr(p);
    }
    if (auto p = std::dynamic_pointer_cast<SetExpr>(expr)) {
      return visitSetExpr(p);
    }
    if (auto p = std::dynamic_pointer_cast<ThisExpr>(expr)) {
      return visitThisExpr(p);
    }
    if (auto p = std::dynamic_pointer_cast<SuperExpr>(expr)) {
      return visitSuperExpr(p);
    }
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
  virtual R visitGetExpr(std::shared_ptr<GetExpr> expr) = 0;
  virtual R visitSetExpr(std::shared_ptr<SetExpr> expr) = 0;
  virtual R visitThisExpr(std::shared_ptr<ThisExpr> expr) = 0;
  virtual R visitSuperExpr(std::shared_ptr<SuperExpr> expr) = 0;
};

#endif // CLOX_EXPR_H
