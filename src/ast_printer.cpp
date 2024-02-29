#include "ast_printer.h"
#include "util.h"

std::string AstPrinter::parenthesize(const std::string &name, const std::vector<SPExpr> &exprList) {
  std::string string;
  string.append("(").append(name);
  for (auto &expr : exprList) {
    string.append(" ");
    string.append(visit(expr));
  }
  string.append(")");
  return string;
}

std::string AstPrinter::visitBinaryExpr(BinaryExpr &expr) {
  return parenthesize(expr.op->lexeme, {expr.left, expr.right});
}

std::string AstPrinter::visitGroupingExpr(GroupingExpr &expr) { return parenthesize("group", {expr.expression}); }

std::string AstPrinter::visitUnaryExpr(UnaryExpr &expr) { return parenthesize(expr.op->lexeme, {expr.right}); }

std::string AstPrinter::visitLiteralExpr(LiteralExpr &expr) { return toString(expr.value, ""); }

std::string AstPrinter::print(const SPExpr &expr) { return visit(expr); }
