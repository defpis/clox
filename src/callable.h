#ifndef CLOX_CALLABLE_H
#define CLOX_CALLABLE_H

#include "environment.h"
#include "interpreter.h"

class Callable;
class Function;
class Class;
class Instance;

using SPCallable = std::shared_ptr<Callable>;
using SPFunction = std::shared_ptr<Function>;
using SPClass = std::shared_ptr<Class>;
using SPInstance = std::shared_ptr<Instance>;

class Stringify {
public:
  virtual ~Stringify() = default;

  virtual std::string toString() = 0;
};

class Callable : public Stringify {
public:
  virtual ~Callable() = default;

  virtual std::size_t arity() = 0;
  virtual std::any call(Interpreter *interpreter, const std::vector<std::any> &arguments) = 0;
};

class Function : public Callable {
private:
  bool isInitializer;

public:
  ~Function() override = default;

  std::size_t arity() override;
  std::any call(Interpreter *interpreter, const std::vector<std::any> &arguments) override;
  std::string toString() override;

  std::shared_ptr<FunStmt> declaration;
  SPEnvironment closure;

  explicit Function(std::shared_ptr<FunStmt> declaration, SPEnvironment closure, bool isInitializer)
      : declaration(std::move(declaration)), closure(std::move(closure)), isInitializer(isInitializer) {}

  SPFunction bind(SPInstance instance);
};

class Clock : public Callable {
public:
  ~Clock() override = default;

  std::size_t arity() override;
  std::any call(Interpreter *interpreter, const std::vector<std::any> &arguments) override;
  std::string toString() override;
};

class Count : public Callable {
private:
  static int count;

public:
  ~Count() override = default;

  std::size_t arity() override;
  std::any call(Interpreter *interpreter, const std::vector<std::any> &arguments) override;
  std::string toString() override;
};

template <typename T, typename U> class Object : public Stringify, public std::enable_shared_from_this<U> {
protected:
  Interpreter *interpreter;
  std::shared_ptr<T> klass;
  std::map<std::string, std::any> fields;

public:
  explicit Object(Interpreter *interpreter, std::shared_ptr<T> klass)
      : interpreter(interpreter), klass(std::move(klass)) {}

  virtual std::any get(SPToken name) = 0;

  virtual std::any set(SPToken name, std::any value) = 0;

  std::any assign(SPToken name, std::any value) {
    SPFunction method = nullptr;
    if (klass) {
      method = klass->findMethod(name->lexeme);
    }
    auto it = fields.find(name->lexeme);

    if (method || it != fields.end()) {
      return set(name, value);
    }

    throw Interpreter::error(name, "Undefined property '" + name->lexeme + "' can't be assign.");
  }

  std::string toString() override {
    if (klass) {
      return "<instance of " + klass->name->lexeme + ">";
    }
    return "<instance wasn't created by class>";
  }
};

class Class : public Callable, public Object<Class, Class> {
public:
  ~Class() override = default;

  SPToken name;
  SPClass superclass;

  std::map<std::string, SPFunction> methods;
  SPFunction findMethod(const std::string &_name);

  std::vector<std::shared_ptr<VarStmt>> variables;
  SPEnvironment closure;

  Class(Interpreter *interpreter, SPToken name, SPClass superclass, std::map<std::string, SPFunction> methods,
        std::vector<std::shared_ptr<VarStmt>> variables, SPEnvironment closure)
      : name(std::move(name)), superclass(std::move(superclass)), methods(std::move(methods)),
        variables(std::move(variables)), closure(std::move(closure)), Object(interpreter, nullptr) {}

  std::size_t arity() override;
  std::any call(Interpreter *interpreter, const std::vector<std::any> &arguments) override;
  std::any get(SPToken name) override;
  std::any set(SPToken name, std::any value) override;
  std::string toString() override;
};

class Instance : public Object<Class, Instance> {
public:
  explicit Instance(Interpreter *interpreter, SPClass klass) : Object(interpreter, std::move(klass)) {}
  std::any get(SPToken name) override;
  std::any set(SPToken name, std::any value) override;
};

#endif // CLOX_CALLABLE_H
