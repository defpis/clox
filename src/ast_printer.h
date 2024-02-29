#ifndef CLOX_AST_PRINTER_H
#define CLOX_AST_PRINTER_H

#include "expr.h"

class AstPrinter : public ExprVisitor<std::string> {
private:
  std::string parenthesize(const std::string &name, const std::vector<SPExpr> &exprList);

  std::string visitBinaryExpr(std::shared_ptr<BinaryExpr> expr) override;
  std::string visitGroupingExpr(std::shared_ptr<GroupingExpr> expr) override;
  std::string visitUnaryExpr(std::shared_ptr<UnaryExpr> expr) override;
  std::string visitLiteralExpr(std::shared_ptr<LiteralExpr> expr) override;
  std::string visitVariableExpr(std::shared_ptr<VariableExpr> expr) override;
  std::string visitAssignExpr(std::shared_ptr<AssignExpr> expr) override;
  std::string visitLogicalExpr(std::shared_ptr<LogicalExpr> expr) override;
  std::string visitCallExpr(std::shared_ptr<CallExpr> expr) override;
  std::string visitGetExpr(std::shared_ptr<GetExpr> expr) override;
  std::string visitSetExpr(std::shared_ptr<SetExpr> expr) override;
  std::string visitThisExpr(std::shared_ptr<ThisExpr> expr) override;
  std::string visitSuperExpr(std::shared_ptr<SuperExpr> expr) override;

public:
  std::string print(SPExpr expr);
};

#endif // CLOX_AST_PRINTER_H
