#ifndef CLOX_STMT_H
#define CLOX_STMT_H

#include "expr.h"
#include <vector>

enum class Modifier {
  NONE,
  STATIC,
  GETTER,
  SETTER,
};

class Stmt {
public:
  virtual ~Stmt() = default;
};

using SPStmt = std::shared_ptr<Stmt>;

class ExprStmt : public Stmt {
public:
  SPExpr expression;

  ~ExprStmt() override = default;

  explicit ExprStmt(SPExpr expression) : expression(std::move(expression)) {}
};

class ReturnStmt : public Stmt {
public:
  SPToken keyword;
  SPExpr value;

  ~ReturnStmt() override = default;

  explicit ReturnStmt(SPToken keyword, SPExpr value) : keyword(std::move(keyword)), value(std::move(value)) {}
};

class PrintStmt : public Stmt {
public:
  SPExpr expression;

  ~PrintStmt() override = default;

  explicit PrintStmt(SPExpr expression) : expression(std::move(expression)) {}
};

class VarStmt : public Stmt {
public:
  SPToken name;
  SPExpr initializer;
  Modifier modifier;

  ~VarStmt() override = default;

  VarStmt(SPToken name, SPExpr initializer, Modifier modifier)
      : name(std::move(name)), initializer(std::move(initializer)), modifier(modifier) {}
};

class BlockStmt : public Stmt {
public:
  std::vector<SPStmt> statements;

  ~BlockStmt() override = default;

  explicit BlockStmt(std::vector<SPStmt> statements) : statements(std::move(statements)) {}
};

class FunStmt : public Stmt {
public:
  SPToken name;
  std::vector<SPToken> params;
  std::shared_ptr<BlockStmt> body;
  Modifier modifier;

  ~FunStmt() override = default;

  explicit FunStmt(SPToken name, std::vector<SPToken> params, std::shared_ptr<BlockStmt> body, Modifier modifier)
      : name(std::move(name)), params(std::move(params)), body(std::move(body)), modifier(modifier) {}
};

struct ClassAttributes {
  std::vector<std::shared_ptr<VarStmt>> variables;
  std::vector<std::shared_ptr<FunStmt>> methods;
};

class ClassStmt : public Stmt {
public:
  SPToken name;
  std::shared_ptr<VariableExpr> superclass;

  ClassAttributes instanceAttributes;
  ClassAttributes staticAttributes;

  ~ClassStmt() override = default;

  ClassStmt(SPToken name, std::shared_ptr<VariableExpr> superclass, ClassAttributes instanceAttributes,
            ClassAttributes staticAttributes)
      : name(std::move(name)), superclass(std::move(superclass)), instanceAttributes(std::move(instanceAttributes)),
        staticAttributes(std::move(staticAttributes)) {}
};

class IfStmt : public Stmt {
public:
  SPExpr condition;
  SPStmt thenBranch;
  SPStmt elseBranch;

  ~IfStmt() override = default;

  IfStmt(SPExpr condition, SPStmt thenBranch, SPStmt elseBranch)
      : condition(std::move(condition)), thenBranch(std::move(thenBranch)), elseBranch(std::move(elseBranch)) {}
};

class WhileStmt : public Stmt {
public:
  SPExpr condition;
  SPStmt body;

  ~WhileStmt() override = default;

  WhileStmt(SPExpr condition, SPStmt body) : condition(std::move(condition)), body(std::move(body)) {}
};

template <typename R> class StmtVisitor {
protected:
  R visitStmt(SPStmt stmt) {
    if (auto p = std::dynamic_pointer_cast<ExprStmt>(stmt)) {
      return visitExprStmt(p);
    }
    if (auto p = std::dynamic_pointer_cast<ReturnStmt>(stmt)) {
      return visitReturnStmt(p);
    }
    if (auto p = std::dynamic_pointer_cast<PrintStmt>(stmt)) {
      return visitPrintStmt(p);
    }
    if (auto p = std::dynamic_pointer_cast<FunStmt>(stmt)) {
      return visitFunStmt(p);
    }
    if (auto p = std::dynamic_pointer_cast<ClassStmt>(stmt)) {
      return visitClassStmt(p);
    }
    if (auto p = std::dynamic_pointer_cast<VarStmt>(stmt)) {
      return visitVarStmt(p);
    }
    if (auto p = std::dynamic_pointer_cast<BlockStmt>(stmt)) {
      return visitBlockStmt(p);
    }
    if (auto p = std::dynamic_pointer_cast<IfStmt>(stmt)) {
      return visitIfStmt(p);
    }
    if (auto p = std::dynamic_pointer_cast<WhileStmt>(stmt)) {
      return visitWhileStmt(p);
    }
    throw std::runtime_error("Unexpected statement type.");
  }

  virtual R visitExprStmt(std::shared_ptr<ExprStmt> stmt) = 0;
  virtual R visitReturnStmt(std::shared_ptr<ReturnStmt> stmt) = 0;
  virtual R visitPrintStmt(std::shared_ptr<PrintStmt> stmt) = 0;
  virtual R visitFunStmt(std::shared_ptr<FunStmt> stmt) = 0;
  virtual R visitClassStmt(std::shared_ptr<ClassStmt> stmt) = 0;
  virtual R visitVarStmt(std::shared_ptr<VarStmt> stmt) = 0;
  virtual R visitBlockStmt(std::shared_ptr<BlockStmt> stmt) = 0;
  virtual R visitIfStmt(std::shared_ptr<IfStmt> stmt) = 0;
  virtual R visitWhileStmt(std::shared_ptr<WhileStmt> stmt) = 0;
};

#endif // CLOX_STMT_H
