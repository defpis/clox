#ifndef CLOX_INTERPRETER_H
#define CLOX_INTERPRETER_H

#include "environment.h"
#include "expr.h"
#include "stmt.h"

class InterpretError : public std::exception {};

class ReturnValue : public std::exception {
public:
  std::any value;
  explicit ReturnValue(std::any value) : value(std::move(value)) {}
};

class Interpreter : public ExprVisitor<std::any>, StmtVisitor<void> {
private:
  std::map<SPExpr, int> locals;
  void reset();

  static void checkNumberOperand(SPToken op, std::any &value);
  static void checkNumberOperands(SPToken op, std::any &left, std::any &right);

  std::any visitBinaryExpr(std::shared_ptr<BinaryExpr> expr) override;
  std::any visitGroupingExpr(std::shared_ptr<GroupingExpr> expr) override;
  std::any visitUnaryExpr(std::shared_ptr<UnaryExpr> expr) override;
  std::any visitLiteralExpr(std::shared_ptr<LiteralExpr> expr) override;
  std::any visitVariableExpr(std::shared_ptr<VariableExpr> expr) override;
  std::any visitAssignExpr(std::shared_ptr<AssignExpr> expr) override;
  std::any visitLogicalExpr(std::shared_ptr<LogicalExpr> expr) override;
  std::any visitCallExpr(std::shared_ptr<CallExpr> expr) override;
  std::any visitGetExpr(std::shared_ptr<GetExpr> expr) override;
  std::any visitSetExpr(std::shared_ptr<SetExpr> expr) override;
  std::any visitThisExpr(std::shared_ptr<ThisExpr> expr) override;
  std::any visitSuperExpr(std::shared_ptr<SuperExpr> expr) override;

  void execute(SPStmt stmt);

  void visitExprStmt(std::shared_ptr<ExprStmt> stmt) override;
  void visitReturnStmt(std::shared_ptr<ReturnStmt> stmt) override;
  void visitPrintStmt(std::shared_ptr<PrintStmt> stmt) override;
  void visitFunStmt(std::shared_ptr<FunStmt> stmt) override;
  void visitClassStmt(std::shared_ptr<ClassStmt> stmt) override;
  void visitVarStmt(std::shared_ptr<VarStmt> stmt) override;
  void visitBlockStmt(std::shared_ptr<BlockStmt> stmt) override;
  void visitIfStmt(std::shared_ptr<IfStmt> stmt) override;
  void visitWhileStmt(std::shared_ptr<WhileStmt> stmt) override;

  Interpreter();

  template <typename T>
  std::any handleCall(std::shared_ptr<CallExpr> expr, std::any callee, std::vector<std::any> &arguments);

public:
  static Interpreter &getInstance();
  Interpreter(const Interpreter &) = delete;
  Interpreter &operator=(const Interpreter &) = delete;

  SPEnvironment globals = std::make_shared<Environment>();
  SPEnvironment environment = globals;

  std::any evaluate(SPExpr expr);
  void executeBlock(std::shared_ptr<BlockStmt> blockStmt, SPEnvironment _environment);

  static InterpretError error(SPToken token, const std::string &message);
  void interpret(std::vector<SPStmt> &statements, std::map<SPExpr, int> &_locals);
};

#endif // CLOX_INTERPRETER_H
