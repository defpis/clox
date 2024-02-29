#ifndef LOX_INTERPRETER_H
#define LOX_INTERPRETER_H

#include "expr.h"

class InterpreterError : public std::exception {
public:
  std::string message;
  explicit InterpreterError(std::string message) : message(std::move(message)) {}
};

class Interpreter : public ExprVisitor<std::any> {
private:
  std::any evaluate(SPExpr &expr);

  static void checkNumberOperand(SPToken &op, std::any &value);
  static void checkNumberOperands(SPToken &op, std::any &left, std::any &right);

  static InterpreterError error(const SPToken &token, const std::string &message);

  std::any visitBinaryExpr(BinaryExpr &expr) override;
  std::any visitGroupingExpr(GroupingExpr &expr) override;
  std::any visitUnaryExpr(UnaryExpr &expr) override;
  std::any visitLiteralExpr(LiteralExpr &expr) override;

public:
  std::optional<std::any> interpret(SPExpr &expr);
};

#endif // LOX_INTERPRETER_H
