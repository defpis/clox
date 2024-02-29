#ifndef LOX_EXPR_H
#define LOX_EXPR_H

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

  explicit LiteralExpr(const std::any &val);
};

template <typename R> class ExprVisitor {
protected:
  R visit(const SPExpr &expr) {
    if (auto *p = dynamic_cast<BinaryExpr *>(expr.get())) {
      return visitBinaryExpr(*p);
    }
    if (auto *p = dynamic_cast<GroupingExpr *>(expr.get())) {
      return visitGroupingExpr(*p);
    }
    if (auto *p = dynamic_cast<UnaryExpr *>(expr.get())) {
      return visitUnaryExpr(*p);
    }
    if (auto *p = dynamic_cast<LiteralExpr *>(expr.get())) {
      return visitLiteralExpr(*p);
    }
    throw std::runtime_error("Unexpected expression type.");
  }

  virtual R visitBinaryExpr(BinaryExpr &expr) = 0;
  virtual R visitGroupingExpr(GroupingExpr &expr) = 0;
  virtual R visitUnaryExpr(UnaryExpr &expr) = 0;
  virtual R visitLiteralExpr(LiteralExpr &expr) = 0;
};

#endif // LOX_EXPR_H
