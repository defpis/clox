#include "interpreter.h"
#include "callable.h"
#include "lox.h"
#include "util.h"
#include <cmath>
#include <iostream>

using namespace util;

void Interpreter::reset() { locals.clear(); }

Interpreter::Interpreter() {
  globals->define("clock", static_cast<SPCallable>(std::make_shared<Clock>()));
  globals->define("count", static_cast<SPCallable>(std::make_shared<Count>()));
}

std::any Interpreter::evaluate(SPExpr expr) { return visitExpr(std::move(expr)); }

void Interpreter::checkNumberOperand(SPToken op, std::any &value) {
  if (isNumber(value)) {
    return;
  }
  throw error(std::move(op), "Operand must be a number.");
}

void Interpreter::checkNumberOperands(SPToken op, std::any &left, std::any &right) {
  if (isNumber(left) && isNumber(right)) {
    return;
  }
  throw error(std::move(op), "Operands must be two numbers.");
}

std::any Interpreter::visitBinaryExpr(std::shared_ptr<BinaryExpr> expr) {
  std::any left = evaluate(expr->left);
  std::any right = evaluate(expr->right);

  switch (expr->op->type) {
    case TokenType::MINUS: {
      checkNumberOperands(expr->op, left, right);
      return toNumber(left, 0) - toNumber(right, 0);
    }
    case TokenType::SLASH: {
      checkNumberOperands(expr->op, left, right);
      double rightValue = toNumber(right, 0);
      if (rightValue == 0) {
        throw error(expr->op, "Division by zero"); // ZeroDivisionError
      }
      return toNumber(left, 0) / rightValue;
    }
    case TokenType::STAR: {
      checkNumberOperands(expr->op, left, right);
      return toNumber(left, 0) * toNumber(right, 0);
    }
    case TokenType::PLUS: {
      if (isNumber(left) && isNumber(right)) {
        return toNumber(left, 0) + toNumber(right, 0);
      }
      // 支持字符串拼接
      if (isString(left) && isString(right)) {
        return toString(left, "") + toString(right, "");
      }
      throw error(expr->op, "Operands must be two numbers or two strings.");
    }
    case TokenType::GREATER: {
      checkNumberOperands(expr->op, left, right);
      return toNumber(left, 0) > toNumber(right, 0);
    }
    case TokenType::GREATER_EQUAL: {
      checkNumberOperands(expr->op, left, right);
      return toNumber(left, 0) >= toNumber(right, 0);
    }
    case TokenType::LESS: {
      checkNumberOperands(expr->op, left, right);
      return toNumber(left, 0) < toNumber(right, 0);
    }
    case TokenType::LESS_EQUAL: {
      checkNumberOperands(expr->op, left, right);
      return toNumber(left, 0) <= toNumber(right, 0);
    }
    case TokenType::BANG_EQUAL: {
      return !isEqual(left, right);
    }
    case TokenType::EQUAL_EQUAL: {
      return isEqual(left, right);
    }
    case TokenType::STAR_STAR: {
      checkNumberOperands(expr->op, left, right);
      return pow(toNumber(left, 0), toNumber(right, 0));
    }
    default: {
    }
  }

  throw error(expr->op, "Unexpected operator type.");
}

std::any Interpreter::visitGroupingExpr(std::shared_ptr<GroupingExpr> expr) { return evaluate(expr->expression); }

std::any Interpreter::visitUnaryExpr(std::shared_ptr<UnaryExpr> expr) {
  std::any right = evaluate(expr->right);

  switch (expr->op->type) {
    case TokenType::BANG: {
      return !toBool(right, false);
    }
    case TokenType::PLUS: {
      checkNumberOperand(expr->op, right);
      return toNumber(right, 0);
    }
    case TokenType::MINUS: {
      checkNumberOperand(expr->op, right);
      return -toNumber(right, 0);
    }
    default: {
    }
  }

  throw error(expr->op, "Unexpected operator type.");
}

std::any Interpreter::visitLiteralExpr(std::shared_ptr<LiteralExpr> expr) { return (expr->value); }

std::any Interpreter::visitVariableExpr(std::shared_ptr<VariableExpr> expr) {
  auto it = locals.find(expr);
  if (it != locals.end()) {
    return environment->getAt(it->second, expr->name->lexeme);
  } else {
    // 需要获取系统内置函数
    return globals->get(expr->name);
  }
}

std::any Interpreter::visitAssignExpr(std::shared_ptr<AssignExpr> expr) {
  std::any original;
  std::any value = evaluate(expr->value);

  auto it = locals.find(expr);
  if (it != locals.end()) {
    original = environment->getAt(it->second, expr->name->lexeme);
    environment->assignAt(it->second, expr->name->lexeme, value);
  } else {
    throw error(expr->name, "This variable can't be found.");
  }

  return expr->returnOriginal ? std::move(original) : std::move(value);
}

std::any Interpreter::visitLogicalExpr(std::shared_ptr<LogicalExpr> expr) {
  std::any left = evaluate(expr->left);

  // or/and
  if (expr->op->type == TokenType::OR) {
    if (toBool(left, false)) {
      return left;
    }
  } else {
    if (!toBool(left, false)) {
      return left;
    }
  }

  return evaluate(expr->right);
}

template <typename T>
std::any Interpreter::handleCall(std::shared_ptr<CallExpr> expr, std::any callee, std::vector<std::any> &arguments) {
  auto callable = std::any_cast<T>(callee);

  if (arguments.size() != callable->arity()) {
    throw error(expr->paren, "Expected " + toString(static_cast<int>(callable->arity()), "") + " arguments but got " +
                                 toString(static_cast<int>(arguments.size()), "") + "."); // std::size_t => int
  }

  return callable->call(this, arguments);
}

std::any Interpreter::visitCallExpr(std::shared_ptr<CallExpr> expr) {
  std::any callee = evaluate(expr->callee);

  std::vector<std::any> arguments;
  arguments.reserve(expr->arguments.size());

  for (auto &argument : expr->arguments) {
    arguments.push_back(evaluate(argument));
  }

  if (callee.type() == typeid(SPCallable)) {
    return handleCall<SPCallable>(expr, callee, arguments);
  }

  if (callee.type() == typeid(SPClass)) {
    return handleCall<SPClass>(expr, callee, arguments);
  }

  throw error(expr->paren, "Can only call functions and classes.");
}

std::any Interpreter::visitGetExpr(std::shared_ptr<GetExpr> expr) {
  std::any object = evaluate(expr->object);

  if (object.type() == typeid(SPClass)) {
    return std::any_cast<SPClass>(object)->get(expr->name);
  }

  if (object.type() == typeid(SPInstance)) {
    return std::any_cast<SPInstance>(object)->get(expr->name);
  }

  throw error(expr->name, "Only instances have properties.");
}

template <typename T> std::any setValue(std::shared_ptr<SetExpr> expr, std::any object, std::any value) {
  auto obj = std::any_cast<T>(object);
  auto original = obj->get(expr->name);
  obj->assign(expr->name, value);
  return expr->returnOriginal ? std::move(original) : std::move(value);
}

std::any Interpreter::visitSetExpr(std::shared_ptr<SetExpr> expr) {
  std::any object = evaluate(expr->object);
  std::any value = evaluate(expr->value);

  if (object.type() == typeid(SPClass)) {
    return setValue<SPClass>(expr, object, value);
  }

  if (object.type() == typeid(SPInstance)) {
    return setValue<SPInstance>(expr, object, value);
  }

  throw error(expr->name, "Only instances have properties.");
}

std::any Interpreter::visitThisExpr(std::shared_ptr<ThisExpr> expr) {
  auto it = locals.find(expr);
  if (it != locals.end()) {
    return environment->getAt(it->second, expr->keyword->lexeme);
  } else {
    throw error(expr->keyword, "Can't find binding.");
  }
}

std::any Interpreter::visitSuperExpr(std::shared_ptr<SuperExpr> expr) {
  auto it = locals.find(expr);
  if (it != locals.end()) {
    auto r1 = environment->getAt(it->second, expr->keyword->lexeme); // "super"
    if (r1.type() != typeid(SPClass)) {
      throw error(expr->keyword, "Unknown error");
    }
    auto superclass = std::any_cast<SPClass>(r1);

    auto r2 = environment->getAt(it->second - 1, "this"); // 直接从更近的env获取this定义
    if (r2.type() != typeid(SPInstance)) {
      throw error(expr->keyword, "Unknown error");
    }
    auto instance = std::any_cast<SPInstance>(r2);

    SPFunction method = superclass->findMethod(expr->method->lexeme);
    if (!method) {
      throw error(expr->method, "Undefined property '" + expr->method->lexeme + "'.");
    }

    return static_cast<SPCallable>(method->bind(instance));
  } else {
    throw error(expr->keyword, "Can't find binding.");
  }
}

void Interpreter::execute(SPStmt stmt) { visitStmt(std::move(stmt)); }

void Interpreter::executeBlock(std::shared_ptr<BlockStmt> blockStmt, SPEnvironment _environment) {
  SPEnvironment previous = environment;
  environment = std::move(_environment);

  auto finally = [this, previous]() { this->environment = previous; };

  // try {
  for (auto &statement : blockStmt->statements) {
    execute(statement);
  }
  // } catch (...) { // catch any error
  //   finally();
  //   throw; // rethrow it!
  // }

  finally();
}

void Interpreter::visitExprStmt(std::shared_ptr<ExprStmt> stmt) { evaluate(stmt->expression); }

void Interpreter::visitReturnStmt(std::shared_ptr<ReturnStmt> stmt) {
  std::any value;

  if (stmt->value) {
    value = evaluate(stmt->value);
  }

  throw ReturnValue(value);
}

void Interpreter::visitPrintStmt(std::shared_ptr<PrintStmt> stmt) {
  std::any value = evaluate(stmt->expression);

  if (value.type() == typeid(SPCallable)) {
    std::cout << std::any_cast<SPCallable>(value)->toString() << std::endl;
    return;
  }

  if (value.type() == typeid(SPClass)) {
    std::cout << std::any_cast<SPClass>(value)->toString() << std::endl;
    return;
  }

  if (value.type() == typeid(SPInstance)) {
    std::cout << std::any_cast<SPInstance>(value)->toString() << std::endl;
    return;
  }

  std::cout << toString(value, "") << std::endl;
}

void Interpreter::visitFunStmt(std::shared_ptr<FunStmt> stmt) {
  auto function = static_cast<SPCallable>(std::make_shared<Function>(stmt, environment, false));
  environment->define(stmt->name->lexeme, function);
}

void Interpreter::visitClassStmt(std::shared_ptr<ClassStmt> stmt) {
  SPClass superclass = nullptr;
  if (stmt->superclass) {
    std::any result = evaluate(stmt->superclass);
    if (result.type() != typeid(SPClass)) {
      throw error(stmt->superclass->name, "Superclass must be a class.");
    }
    superclass = std::any_cast<SPClass>(result);
  }

  SPEnvironment closure = std::make_shared<Environment>(environment);

  if (stmt->superclass) {
    closure->define("super", superclass);
    closure = std::make_shared<Environment>(closure); // 额外包裹一层，对应resolver实现
  }

  std::map<std::string, SPFunction> methods;
  for (auto &method : stmt->instanceAttributes.methods) {
    SPFunction function = std::make_shared<Function>(method, closure, method->name->lexeme == "init");
    methods[method->name->lexeme] = function;
  }

  auto klass =
      std::make_shared<Class>(this, stmt->name, superclass, methods, stmt->instanceAttributes.variables, closure);

  for (auto &variable : stmt->staticAttributes.variables) {
    std::any value;
    if (variable->initializer) {
      value = evaluate(variable->initializer);
    }
    klass->set(variable->name, value);
  }

  for (auto &method : stmt->staticAttributes.methods) {
    auto function = static_cast<SPCallable>(std::make_shared<Function>(method, environment, false));
    klass->set(method->name, function);
  }

  environment->define(stmt->name->lexeme, klass);
}

void Interpreter::visitVarStmt(std::shared_ptr<VarStmt> stmt) {
  std::any value;
  if (stmt->initializer) {
    value = evaluate(stmt->initializer);
  }
  environment->define(stmt->name->lexeme, value);
}

void Interpreter::visitBlockStmt(std::shared_ptr<BlockStmt> stmt) {
  executeBlock(stmt, std::make_shared<Environment>(environment));
}

void Interpreter::visitIfStmt(std::shared_ptr<IfStmt> stmt) {
  if (toBool(evaluate(stmt->condition), false)) {
    execute(stmt->thenBranch);
  } else if (stmt->elseBranch) {
    execute(stmt->elseBranch);
  }
}

void Interpreter::visitWhileStmt(std::shared_ptr<WhileStmt> stmt) {
  while (toBool(evaluate(stmt->condition), false)) {
    execute(stmt->body);
  }
}

InterpretError Interpreter::error(SPToken token, const std::string &message) {
  lox::error(std::move(token), message);
  return {};
}

void Interpreter::interpret(std::vector<SPStmt> &statements, std::map<SPExpr, int> &_locals) {
  reset();
  locals = _locals;
  for (auto &statement : statements) {
    execute(statement);
  }
}

Interpreter &Interpreter::getInstance() {
  static Interpreter instance;
  return instance;
}
