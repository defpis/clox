#include "callable.h"
#include <chrono>
#include <iostream>

std::size_t Function::arity() { return declaration->params.size(); }

std::any Function::call(Interpreter *interpreter, const std::vector<std::any> &arguments) {
  SPEnvironment environment = std::make_shared<Environment>(closure);

  for (int i = 0; i < declaration->params.size(); i++) {
    environment->define(declaration->params.at(i)->lexeme, arguments.at(i));
  }

  try {
    interpreter->executeBlock(declaration->body, environment);
  } catch (ReturnValue &rv) {
    if (isInitializer) {
      return closure->getAt(0, "this");
    }

    return rv.value;
  }

  if (isInitializer) {
    return closure->getAt(0, "this");
  }

  return nullptr;
}

std::string Function::toString() { return "<function " + declaration->name->lexeme + ">"; }

SPFunction Function::bind(SPInstance instance) {
  closure->define("this", instance); // redefined "this"
  return std::make_shared<Function>(declaration, closure, isInitializer);
}

std::size_t Clock::arity() { return 0; }

std::any Clock::call(Interpreter *interpreter, const std::vector<std::any> &arguments) {
  auto now = std::chrono::system_clock::now();
  return std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
}

std::string Clock::toString() { return "<function native-clock>"; }

int Count::count = 0;

std::size_t Count::arity() { return 0; }

std::any Count::call(Interpreter *interpreter, const std::vector<std::any> &arguments) { return ++count; }

std::string Count::toString() { return "<function native-count>"; }

std::size_t Class::arity() {
  SPFunction initializer = findMethod("init");
  if (initializer) {
    return initializer->arity();
  }
  return 0;
}

std::any Class::call(Interpreter *interpreter, const std::vector<std::any> &arguments) {
  SPInstance instance = std::make_shared<Instance>(interpreter, std::dynamic_pointer_cast<Class>(shared_from_this()));

  SPFunction initializer = findMethod("init");
  if (initializer) {
    initializer->bind(instance)->call(interpreter, arguments);
  }

  // 调用var a = A()生成一个实例时，当前env中的this的会被设置为最新构建的实例
  // 调用method之前需要bind绑定当前调用对象，this随调用对象在env中不断重新绑定
  closure->define("this", instance);

  SPEnvironment previous = interpreter->environment;
  interpreter->environment = closure;

  auto finally = [interpreter, previous]() { interpreter->environment = previous; };

  try {
    for (auto &variable : variables) {
      std::any value;
      if (variable->initializer) {
        value = interpreter->evaluate(variable->initializer);
      }
      instance->set(variable->name, value);
    }
  } catch (...) {
    finally();
    throw;
  }

  finally();

  return instance;
}

SPFunction Class::findMethod(const std::string &_name) {
  auto it = methods.find(_name);
  if (it != methods.end()) {
    return it->second;
  }

  if (superclass) {
    return superclass->findMethod(_name);
  }

  return nullptr;
}

std::string Class::toString() { return "<class " + name->lexeme + ">"; }

std::any Class::get(SPToken name) {
  auto it = fields.find(name->lexeme);
  if (it != fields.end()) {
    return it->second;
  }

  if (klass) {
    SPFunction method = klass->findMethod(name->lexeme);
    if (method) {
      auto callable = static_cast<SPCallable>(method->bind(std::dynamic_pointer_cast<Instance>(shared_from_this())));
      if (method->declaration->modifier == Modifier::GETTER) {
        callable->call(interpreter, {});
      }
      return callable;
    }
  }

  throw Interpreter::error(name, "Undefined property '" + name->lexeme + "' can't be get.");
}

std::any Class::set(SPToken name, std::any value) {
  auto it = fields.find(name->lexeme);
  if (it != fields.end()) {
    // update
    fields[name->lexeme] = value;
    return value;
  }

  if (klass) {
    SPFunction method = klass->findMethod(name->lexeme);
    if (method && method->declaration->modifier == Modifier::SETTER) {
      return static_cast<SPCallable>(method->bind(std::dynamic_pointer_cast<Instance>(shared_from_this())))
          ->call(interpreter, {value});
    }
  }

  // create
  fields[name->lexeme] = value;
  return value;
}

std::any Instance::get(SPToken name) {
  auto it = fields.find(name->lexeme);
  if (it != fields.end()) {
    return it->second;
  }

  if (klass) {
    SPFunction method = klass->findMethod(name->lexeme);
    if (method) {
      auto callable = static_cast<SPCallable>(method->bind(shared_from_this()));
      if (method->declaration->modifier == Modifier::GETTER) {
        return callable->call(interpreter, {});
      }
      return callable;
    }
  }

  throw Interpreter::error(name, "Undefined property '" + name->lexeme + "' can't be get.");
}

std::any Instance::set(SPToken name, std::any value) {
  auto it = fields.find(name->lexeme);
  if (it != fields.end()) {
    // update
    fields[name->lexeme] = value;
    return value;
  }

  if (klass) {
    SPFunction method = klass->findMethod(name->lexeme);
    if (method && method->declaration->modifier == Modifier::SETTER) {
      return static_cast<SPCallable>(method->bind(shared_from_this()))->call(interpreter, {value});
    }
  }

  // create
  fields[name->lexeme] = value;
  return value;
}