#include "resolver.h"
#include "lox.h"
#include <iostream>

void Resolver::reset() { locals.clear(); }

void Resolver::visitVariableExpr(std::shared_ptr<VariableExpr> expr) {
  // 不能在未定义情况下使用，比如变量定义初始化表达式包含自身
  if (!scopes.empty()) {
    ScopeData *found = findInScope(scopes.back(), expr->name);
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

void Resolver::visitThisExpr(std::shared_ptr<ThisExpr> expr) {
  if (currentClass == ClassType::NONE) {
    throw error(expr->keyword, "Can't use 'this' outside of a class.");
  }

  if (currentStatic == StaticType::CLASS) {
    throw error(expr->keyword, "Static attribute can't use 'this'.");
  }

  resolveLocal(expr, expr->keyword);
}

void Resolver::visitSuperExpr(std::shared_ptr<SuperExpr> expr) {
  if (currentClass == ClassType::NONE) {
    throw error(expr->keyword, "Can't use 'super' outside of a class.");
  }
  if (currentClass != ClassType::SUBCLASS) {
    throw error(expr->keyword, "Can't use 'super' in a class with no superclass.");
  }
  resolveLocal(expr, expr->keyword);
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

  if (currentClass == ClassType::NONE && stmt->modifier != Modifier::NONE) {
    throw error(stmt->name, "Only class methods can be decorated.");
  }

  resolveFunction(stmt, FunctionType::FUNCTION);
}

void Resolver::visitClassStmt(std::shared_ptr<ClassStmt> stmt) {
  ClassType enclosingClass = currentClass;
  currentClass = ClassType::CLASS;

  declare(stmt->name);

  if (stmt->superclass && stmt->name->lexeme == stmt->superclass->name->lexeme) {
    throw error(stmt->superclass->name, "A class can't inherit from itself.");
  }

  if (stmt->superclass) {
    currentClass = ClassType::SUBCLASS;
    resolve(stmt->superclass);
  }

  // 允许在上下文中注入super，像this一样的操作
  if (stmt->superclass) {
    beginScope();
    Scope &scope = scopes.back();
    scope.emplace("super", ScopeData{stmt->name, true, true});
  }

  beginScope();
  Scope &scope = scopes.back();
  scope.emplace("this", ScopeData{stmt->name, true, true});

  for (auto &variable : stmt->instanceAttributes.variables) {
    if (variable->initializer) {
      resolve(variable->initializer);
    }
  }

  for (auto &method : stmt->instanceAttributes.methods) {
    FunctionType type = FunctionType::METHOD;
    if (method->name->lexeme == "init") {
      type = FunctionType::INITIALIZER;
    }
    resolveFunction(method, type);
  }

  endScope();

  if (stmt->superclass) {
    endScope();
  }

  // 静态方法不能调用this和super
  StaticType enclosingStatic = currentStatic;
  currentStatic = StaticType::CLASS;

  for (auto &variable : stmt->staticAttributes.variables) {
    if (variable->initializer) {
      resolve(variable->initializer);
    }
  }

  for (auto &method : stmt->staticAttributes.methods) {
    FunctionType type = FunctionType::METHOD;
    if (method->name->lexeme == "init") {
      throw error(method->name, "The init method is a class constructor and can't be static.");
    }
    resolveFunction(method, type);
  }

  currentStatic = enclosingStatic;

  define(stmt->name);

  currentClass = enclosingClass;
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
    if (currentFunction == FunctionType::INITIALIZER) {
      throw error(stmt->keyword, "Can't return a value from an initializer.");
    }

    resolve(stmt->value);
  }
}

void Resolver::visitWhileStmt(std::shared_ptr<WhileStmt> stmt) {
  resolve(stmt->condition);
  resolve(stmt->body);
}

void Resolver::beginScope() { scopes.emplace_back(); }

void Resolver::endScope() {
  Scope &scope = scopes.back();

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
    Scope &scope = scopes.at(i);
    ScopeData *found = findInScope(scope, name);
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

  Scope &scope = scopes.back();
  ScopeData *found = findInScope(scope, name);

  if (!found) { // 未声明
    scope.emplace(name->lexeme, ScopeData{
                                    name,
                                    false,
                                    false,
                                }); // 声明
  } else {
    throw error(name, "Already declared a variable with this name in this scope.");
  }
}

void Resolver::define(SPToken name) {
  if (scopes.empty()) {
    return;
  }

  Scope &scope = scopes.back();
  ScopeData *found = findInScope(scope, std::move(name));

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
