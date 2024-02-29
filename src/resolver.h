#ifndef CLOX_RESOLVER_H
#define CLOX_RESOLVER_H

#include "expr.h"
#include "interpreter.h"
#include "stmt.h"
#include "token.h"
#include <deque>
#include <map>

enum class FunctionType { NONE, FUNCTION, METHOD, INITIALIZER };
enum class ClassType { NONE, CLASS, SUBCLASS };
enum class StaticType { NONE, CLASS };

struct ScopeData {
  SPToken name;
  bool defined;
  bool used;
};

class Scope : public std::map<std::string, ScopeData> {};

class ResolverError : public std::exception {};

class Resolver : public ExprVisitor<void>, StmtVisitor<void> {
private:
  FunctionType currentFunction = FunctionType::NONE;
  ClassType currentClass = ClassType::NONE;
  StaticType currentStatic = StaticType::NONE;

  std::map<SPExpr, int> locals;
  void reset();

  void visitBinaryExpr(std::shared_ptr<BinaryExpr> expr) override;
  void visitGroupingExpr(std::shared_ptr<GroupingExpr> expr) override;
  void visitUnaryExpr(std::shared_ptr<UnaryExpr> expr) override;
  void visitLiteralExpr(std::shared_ptr<LiteralExpr> expr) override;
  void visitVariableExpr(std::shared_ptr<VariableExpr> expr) override;
  void visitAssignExpr(std::shared_ptr<AssignExpr> expr) override;
  void visitLogicalExpr(std::shared_ptr<LogicalExpr> expr) override;
  void visitCallExpr(std::shared_ptr<CallExpr> expr) override;
  void visitGetExpr(std::shared_ptr<GetExpr> expr) override;
  void visitSetExpr(std::shared_ptr<SetExpr> expr) override;
  void visitThisExpr(std::shared_ptr<ThisExpr> expr) override;
  void visitSuperExpr(std::shared_ptr<SuperExpr> expr) override;

  void visitExprStmt(std::shared_ptr<ExprStmt> stmt) override;
  void visitReturnStmt(std::shared_ptr<ReturnStmt> stmt) override;
  void visitPrintStmt(std::shared_ptr<PrintStmt> stmt) override;
  void visitFunStmt(std::shared_ptr<FunStmt> stmt) override;
  void visitClassStmt(std::shared_ptr<ClassStmt> stmt) override;
  void visitVarStmt(std::shared_ptr<VarStmt> stmt) override;
  void visitBlockStmt(std::shared_ptr<BlockStmt> stmt) override;
  void visitIfStmt(std::shared_ptr<IfStmt> stmt) override;
  void visitWhileStmt(std::shared_ptr<WhileStmt> stmt) override;

  void beginScope();
  void endScope();

  void resolve(SPStmt stmt);
  void resolve(SPExpr expr);

  void resolveLocal(SPExpr expr, SPToken name);
  void resolveFunction(std::shared_ptr<FunStmt> function, FunctionType type);

  std::deque<Scope> scopes;
  static ScopeData *findInScope(Scope &scope, SPToken name);

  void declare(SPToken name);
  void define(SPToken name);

  Resolver() = default;

public:
  static Resolver &getInstance();
  Resolver(const Resolver &) = delete;
  Resolver &operator=(const Resolver &) = delete;

  static ResolverError error(SPToken token, const std::string &message);
  static void warn(SPToken token, const std::string &message);

  std::map<SPExpr, int> resolve(std::vector<SPStmt> &statements);
};

#endif // CLOX_RESOLVER_H
