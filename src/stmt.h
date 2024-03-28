#ifndef CLOX_STMT_H
#define CLOX_STMT_H

#include "expr.h"

class Stmt {
public:
  virtual ~Stmt() = default;
};

using SPStmt = std::shared_ptr<Stmt>;

class ExpressionStmt : public Stmt {
public:
  SPExpr expression = nullptr;

  ~ExpressionStmt() override = default;

  explicit ExpressionStmt(SPExpr expression) : expression(std::move(expression)) {}
};

class PrintStmt : public Stmt {
public:
  SPExpr expression;

  ~PrintStmt() override = default;

  explicit PrintStmt(SPExpr expression) : expression(std::move(expression)) {}
};

template <typename R> class StmtVisitor {
protected:
  R visitStmt(const SPStmt &stmt) {
    if (auto *p = dynamic_cast<ExpressionStmt *>(stmt.get())) {
      return visitExpressionStmt(*p);
    }
    if (auto *p = dynamic_cast<PrintStmt *>(stmt.get())) {
      return visitPrintStmt(*p);
    }
    throw std::runtime_error("Unexpected statement type.");
  }

  virtual R visitExpressionStmt(ExpressionStmt &stmt) = 0;
  virtual R visitPrintStmt(PrintStmt &stmt) = 0;
};

#endif // CLOX_STMT_H
