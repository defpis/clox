#ifndef CLOX_INTERPRETER_H
#define CLOX_INTERPRETER_H

#include "expr.h"
#include "stmt.h"

class InterpreterError : public std::exception {
public:
  std::string message;
  explicit InterpreterError(std::string message) : message(std::move(message)) {}
};

class Interpreter : public ExprVisitor<std::any>, StmtVisitor<void> {
private:
  std::any evaluate(SPExpr &expr);

  static void checkNumberOperand(SPToken &op, std::any &value);
  static void checkNumberOperands(SPToken &op, std::any &left, std::any &right);

  static InterpreterError error(const SPToken &token, const std::string &message);

  std::any visitBinaryExpr(BinaryExpr &expr) override;
  std::any visitGroupingExpr(GroupingExpr &expr) override;
  std::any visitUnaryExpr(UnaryExpr &expr) override;
  std::any visitLiteralExpr(LiteralExpr &expr) override;

  void execute(SPStmt &stmt);

  void visitExpressionStmt(ExpressionStmt &stmt) override;
  void visitPrintStmt(PrintStmt &stmt) override;

public:
  void interpret(std::vector<SPStmt> statements);
};

#endif // CLOX_INTERPRETER_H
