#ifndef CLOX_CALLABLE_H
#define CLOX_CALLABLE_H

#include "environment.h"
#include "interpreter.h"

class Object {
public:
  virtual std::string toString() = 0;
};

class Callable : public Object {
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
  std::string toString() override;

  std::shared_ptr<FunStmt> declaration;
  SPEnvironment closure;

  explicit Function(std::shared_ptr<FunStmt> declaration, SPEnvironment closure)
      : declaration(std::move(declaration)), closure(std::move(closure)) {}
};

class Clock : public Callable {
public:
  ~Clock() override = default;

  std::size_t arity() override;
  std::any call(Interpreter *interpreter, std::vector<std::any> &arguments) override;
  std::string toString() override;
};

class Count : public Callable {
private:
  static int count;

public:
  ~Count() override = default;

  std::size_t arity() override;
  std::any call(Interpreter *interpreter, std::vector<std::any> &arguments) override;
  std::string toString() override;
};

class Class : public Callable, public std::enable_shared_from_this<Class> {
public:
  ~Class() override = default;

  SPToken name;
  explicit Class(SPToken name) : name(std::move(name)) {}

  std::size_t arity() override;
  std::any call(Interpreter *interpreter, std::vector<std::any> &arguments) override;
  std::string toString() override;
};

class Instance : public Object {
private:
  std::map<std::string, std::any> fields;

public:
  std::shared_ptr<Class> klass;
  explicit Instance(std::shared_ptr<Class> klass) : klass(std::move(klass)) {}
  std::string toString() override;

  std::any get(SPToken name);
  std::any set(SPToken name, std::any value);
};

using SPInstance = std::shared_ptr<Instance>;

#endif // CLOX_CALLABLE_H
