#include "ast_printer.h"
#include "util.h"

std::string AstPrinter::parenthesize(const std::string &name, const std::vector<SPExpr> &exprList) {
  std::string string;
  string.append("(").append(name);
  for (auto &expr : exprList) {
    string.append(" ");
    string.append(visitExpr(expr));
  }
  string.append(")");
  return string;
}

std::string AstPrinter::visitBinaryExpr(std::shared_ptr<BinaryExpr> expr) {
  return parenthesize(expr->op->lexeme, {expr->left, expr->right});
}

std::string AstPrinter::visitGroupingExpr(std::shared_ptr<GroupingExpr> expr) {
  return parenthesize("group", {expr->expression});
}

std::string AstPrinter::visitUnaryExpr(std::shared_ptr<UnaryExpr> expr) {
  return parenthesize(expr->op->lexeme, {expr->right});
}

std::string AstPrinter::visitLiteralExpr(std::shared_ptr<LiteralExpr> expr) { return util::toString(expr->value, ""); }

std::string AstPrinter::visitVariableExpr(std::shared_ptr<VariableExpr> expr) { return ""; }

std::string AstPrinter::visitAssignExpr(std::shared_ptr<AssignExpr> expr) { return ""; }

std::string AstPrinter::visitLogicalExpr(std::shared_ptr<LogicalExpr> expr) { return ""; }

std::string AstPrinter::visitCallExpr(std::shared_ptr<CallExpr> expr) { return ""; }

std::string AstPrinter::visitGetExpr(std::shared_ptr<GetExpr> expr) { return ""; }

std::string AstPrinter::visitSetExpr(std::shared_ptr<SetExpr> expr) { return ""; }

std::string AstPrinter::visitThisExpr(std::shared_ptr<ThisExpr> expr) { return ""; }

std::string AstPrinter::visitSuperExpr(std::shared_ptr<SuperExpr> expr) { return ""; }

std::string AstPrinter::print(SPExpr expr) { return visitExpr(std::move(expr)); }
