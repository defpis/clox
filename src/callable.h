#ifndef CLOX_CALLABLE_H
#define CLOX_CALLABLE_H

#include "environment.h"
#include "interpreter.h"

class Callable {
public:
  virtual ~Callable() = default;

  virtual std::size_t arity() = 0;
  virtual std::any call(Interpreter *interpreter, std::vector<std::any> &arguments) = 0;
};

using SPCallable = std::shared_ptr<Callable>;

class Function : public Callable {
public:
  ~Function() override = default;

  std::size_t arity() override;
  std::any call(Interpreter *interpreter, std::vector<std::any> &arguments) override;

  std::shared_ptr<FunctionStmt> declaration;
  SPEnvironment closure;

  explicit Function(std::shared_ptr<FunctionStmt> declaration, SPEnvironment closure)
      : declaration(std::move(declaration)), closure(std::move(closure)) {}
};

class Clock : public Callable {
public:
  ~Clock() override = default;

  std::size_t arity() override;
  std::any call(Interpreter *interpreter, std::vector<std::any> &arguments) override;
};

class Count : public Callable {
private:
  static int count;

public:
  ~Count() override = default;

  std::size_t arity() override;
  std::any call(Interpreter *interpreter, std::vector<std::any> &arguments) override;
};

#endif // CLOX_CALLABLE_H
