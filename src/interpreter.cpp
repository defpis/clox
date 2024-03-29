#include "interpreter.h"
#include "lox.h"
#include "util.h"
#include <iostream>

using namespace util;

std::any Interpreter::evaluate(SPExpr &expr) { return visitExpr(expr); }

void Interpreter::checkNumberOperand(SPToken &op, std::any &value) {
  if (isNumber(value)) {
    return;
  }
  throw error(op, "Operand must be a number.");
}

void Interpreter::checkNumberOperands(SPToken &op, std::any &left, std::any &right) {
  if (isNumber(left) && isNumber(right)) {
    return;
  }
  throw error(op, "Operands must be two numbers.");
}

InterpreterError Interpreter::error(const SPToken &token, const std::string &message) {
  lox::error(token, message);
  return InterpreterError(message);
}

std::any Interpreter::visitBinaryExpr(BinaryExpr &expr) {
  std::any left = evaluate(expr.left);
  std::any right = evaluate(expr.right);

  switch (expr.op->type) {
    case TokenType::MINUS: {
      checkNumberOperands(expr.op, left, right);
      return toNumber(left, 0) - toNumber(right, 0);
    }
    case TokenType::SLASH: {
      checkNumberOperands(expr.op, left, right);
      double rightValue = toNumber(right, 0);
      if (rightValue == 0) {
        throw error(expr.op, "Division by zero"); // ZeroDivisionError
      }
      return toNumber(left, 0) / rightValue;
    }
    case TokenType::STAR: {
      checkNumberOperands(expr.op, left, right);
      return toNumber(left, 0) * toNumber(right, 0);
    }
    case TokenType::PLUS: {
      if (isNumber(left) && isNumber(right)) {
        return toNumber(left, 0) + toNumber(right, 0);
      }
      // 支持字符串拼接
      if (isString(left) && isString(right)) {
        return joinString({std::any_cast<std::string>(left), std::any_cast<std::string>(right)}, "");
      }
      throw error(expr.op, "Operands must be two numbers or two strings.");
    }
    case TokenType::GREATER: {
      checkNumberOperands(expr.op, left, right);
      return toNumber(left, 0) > toNumber(right, 0);
    }
    case TokenType::GREATER_EQUAL: {
      checkNumberOperands(expr.op, left, right);
      return toNumber(left, 0) >= toNumber(right, 0);
    }
    case TokenType::LESS: {
      checkNumberOperands(expr.op, left, right);
      return toNumber(left, 0) < toNumber(right, 0);
    }
    case TokenType::LESS_EQUAL: {
      checkNumberOperands(expr.op, left, right);
      return toNumber(left, 0) <= toNumber(right, 0);
    }
    case TokenType::BANG_EQUAL: {
      return !isEqual(left, right);
    }
    case TokenType::EQUAL_EQUAL: {
      return isEqual(left, right);
    }
    default: {
    }
  }

  throw error(expr.op, "Unexpected operator type.");
}

std::any Interpreter::visitGroupingExpr(GroupingExpr &expr) { return evaluate(expr.expression); }

std::any Interpreter::visitUnaryExpr(UnaryExpr &expr) {
  std::any right = evaluate(expr.right);

  switch (expr.op->type) {
    case TokenType::BANG: {
      return !toBool(right, false);
    }
    case TokenType::MINUS: {
      checkNumberOperand(expr.op, right);
      return -toNumber(right, 0);
    }
    default: {
    }
  }

  throw error(expr.op, "Unexpected operator type.");
}

std::any Interpreter::visitLiteralExpr(LiteralExpr &expr) { return expr.value; }

void Interpreter::execute(SPStmt &stmt) { visitStmt(stmt); }

void Interpreter::visitExpressionStmt(ExpressionStmt &stmt) { evaluate(stmt.expression); }

void Interpreter::visitPrintStmt(PrintStmt &stmt) {
  std::any value = evaluate(stmt.expression);
  std::cout << toString(value, "") << std::endl;
}

void Interpreter::interpret(std::vector<SPStmt> statements) {
  for (auto &statement : statements) {
    execute(statement);
  }
}
