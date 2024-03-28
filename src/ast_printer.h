#ifndef CLOX_AST_PRINTER_H
#define CLOX_AST_PRINTER_H

#include "expr.h"

class AstPrinter : public ExprVisitor<std::string> {
private:
  std::string parenthesize(const std::string &name, const std::vector<SPExpr> &exprList);

  std::string visitBinaryExpr(BinaryExpr &expr) override;
  std::string visitGroupingExpr(GroupingExpr &expr) override;
  std::string visitUnaryExpr(UnaryExpr &expr) override;
  std::string visitLiteralExpr(LiteralExpr &expr) override;

public:
  std::string print(const SPExpr &expr);
};

#endif // CLOX_AST_PRINTER_H
