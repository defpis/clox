#include "resolver.h"
#include "lox.h"
#include <iostream>

void Resolver::reset() { locals.clear(); }

void Resolver::visitVariableExpr(std::shared_ptr<VariableExpr> expr) {
  // 不能在未定义情况下使用，比如变量定义初始化表达式包含自身
  if (!scopes.empty()) {
    auto found = findInScope(scopes.back(), expr->name);
    if (found && !found->defined) {
      throw error(expr->name, "Can't read local variable in its own initializer.");
    }
  }

  resolveLocal(expr, expr->name);
}

void Resolver::visitAssignExpr(std::shared_ptr<AssignExpr> expr) {
  resolve(expr->value);
  resolveLocal(expr, expr->name);
}

void Resolver::visitBinaryExpr(std::shared_ptr<BinaryExpr> expr) {
  resolve(expr->left);
  resolve(expr->right);
}

void Resolver::visitGroupingExpr(std::shared_ptr<GroupingExpr> expr) { resolve(expr->expression); }

void Resolver::visitUnaryExpr(std::shared_ptr<UnaryExpr> expr) { resolve(expr->right); }

void Resolver::visitLiteralExpr(std::shared_ptr<LiteralExpr> expr) {}

void Resolver::visitLogicalExpr(std::shared_ptr<LogicalExpr> expr) {
  resolve(expr->left);
  resolve(expr->right);
}

void Resolver::visitCallExpr(std::shared_ptr<CallExpr> expr) {
  resolve(expr->callee);

  for (auto &argument : expr->arguments) {
    resolve(argument);
  }
}

void Resolver::visitGetExpr(std::shared_ptr<GetExpr> expr) { resolve(expr->object); }

void Resolver::visitSetExpr(std::shared_ptr<SetExpr> expr) {
  resolve(expr->object);
  resolve(expr->value);
}

void Resolver::visitVarStmt(std::shared_ptr<VarStmt> stmt) {
  declare(stmt->name);
  if (stmt->initializer) {
    resolve(stmt->initializer);
  }
  define(stmt->name);
}

void Resolver::visitBlockStmt(std::shared_ptr<BlockStmt> stmt) {
  beginScope();
  for (auto &statement : stmt->statements) {
    resolve(statement);
  }
  endScope();
}

void Resolver::visitFunStmt(std::shared_ptr<FunStmt> stmt) {
  declare(stmt->name);
  define(stmt->name);

  resolveFunction(stmt, FunctionType::FUNCTION);
}

void Resolver::visitClassStmt(std::shared_ptr<ClassStmt> stmt) {
  declare(stmt->name);
  define(stmt->name);
}

void Resolver::visitExprStmt(std::shared_ptr<ExprStmt> stmt) { resolve(stmt->expression); }

void Resolver::visitIfStmt(std::shared_ptr<IfStmt> stmt) {
  resolve(stmt->condition);
  resolve(stmt->thenBranch);
  if (stmt->elseBranch) {
    resolve(stmt->elseBranch);
  }
}

void Resolver::visitPrintStmt(std::shared_ptr<PrintStmt> stmt) { resolve(stmt->expression); }

void Resolver::visitReturnStmt(std::shared_ptr<ReturnStmt> stmt) {
  if (currentFunction == FunctionType::NONE) {
    throw error(stmt->keyword, "Can't return from top-level code.");
  }

  if (stmt->value) {
    resolve(stmt->value);
  }
}

void Resolver::visitWhileStmt(std::shared_ptr<WhileStmt> stmt) {
  resolve(stmt->condition);
  resolve(stmt->body);
}

void Resolver::beginScope() { scopes.emplace_back(); }

void Resolver::endScope() {
  auto &scope = scopes.back();

  for (auto &[key, value] : scope) {
    if (!value.used) {
      warn(value.name, "Variable unused.");
    }
  }

  scopes.pop_back();
}

void Resolver::resolve(SPStmt stmt) { visitStmt(std::move(stmt)); }

void Resolver::resolve(SPExpr expr) { visitExpr(std::move(expr)); }

void Resolver::resolveLocal(SPExpr expr, SPToken name) {
  auto size = static_cast<int>(scopes.size()); // 原本的 unsigned long 不转成 int 会导致从零减一后变为一个很大的正值
  for (int i = size - 1; i >= 0; i--) {
    auto &scope = scopes.at(i);
    auto found = findInScope(scope, name);
    if (found) {
      locals[expr] = size - 1 - i;
      found->used = true;
      return;
    }
  }
}

void Resolver::resolveFunction(std::shared_ptr<FunStmt> function, FunctionType type) {
  FunctionType enclosingFunction = currentFunction;
  currentFunction = type;

  beginScope();
  for (auto &param : function->params) {
    declare(param);
    define(param);
  }
  // resolve(function->body);
  for (auto &statement : function->body->statements) {
    resolve(statement);
  }
  endScope();

  currentFunction = enclosingFunction;
}

ScopeData *Resolver::findInScope(Scope &scope, SPToken name) {
  auto it = scope.find(name->lexeme);
  if (it != scope.end()) {
    return &it->second;
  } else {
    return nullptr;
  }
}

void Resolver::declare(SPToken name) {
  if (scopes.empty()) {
    return;
  }

  auto &scope = scopes.back();
  auto found = findInScope(scope, name);

  if (!found) { // 未声明
    scope.emplace(name->lexeme, ScopeData{
                                    name,
                                    false,
                                    false,
                                }); // 声明
  } else {
    throw error(name, "Already a variable with this name in this scope.");
  }
}

void Resolver::define(SPToken name) {
  if (scopes.empty()) {
    return;
  }

  auto &scope = scopes.back();
  auto found = findInScope(scope, std::move(name));

  if (found) {             // 已声明但未定义
    found->defined = true; // 定义
  }
}

ResolverError Resolver::error(SPToken token, const std::string &message) {
  lox::error(std::move(token), message);
  return {};
}

void Resolver::warn(SPToken token, const std::string &message) { lox::warn(std::move(token), message); }

std::map<SPExpr, int> Resolver::resolve(std::vector<SPStmt> &statements) {
  reset();
  beginScope();
  for (auto &statement : statements) {
    resolve(statement);
  }
  endScope();
  return locals;
}

Resolver &Resolver::getInstance() {
  static Resolver instance;
  return instance;
}
