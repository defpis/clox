#include "callable.h"
#include <chrono>

std::size_t Function::arity() { return declaration->params.size(); }

std::any Function::call(Interpreter *interpreter, std::vector<std::any> &arguments) {
  SPEnvironment environment = std::make_shared<Environment>(closure);

  for (int i = 0; i < declaration->params.size(); i++) {
    environment->define(declaration->params.at(i)->lexeme, arguments.at(i));
  }

  try {
    interpreter->executeBlock(declaration->body, environment);
  } catch (ReturnValue &rv) {
    return rv.value;
  }

  return nullptr;
}

std::string Function::toString() { return "<function " + declaration->name->lexeme + ">"; }

std::size_t Clock::arity() { return 0; }

std::any Clock::call(Interpreter *interpreter, std::vector<std::any> &arguments) {
  auto now = std::chrono::system_clock::now();
  return std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
}

std::string Clock::toString() { return "<function native-clock>"; }

int Count::count = 0;

std::size_t Count::arity() { return 0; }

std::any Count::call(Interpreter *interpreter, std::vector<std::any> &arguments) { return ++count; }

std::string Count::toString() { return "<function native-count>"; }

std::size_t Class::arity() { return 0; }

std::any Class::call(Interpreter *interpreter, std::vector<std::any> &arguments) {
  return std::make_shared<Instance>(shared_from_this());
}

std::string Class::toString() { return "<class " + name->lexeme + ">"; }

std::string Instance::toString() { return "<instance of " + klass->name->lexeme + ">"; }

std::any Instance::get(SPToken name) {
  auto it = fields.find(name->lexeme);
  if (it != fields.end()) {
    return it->second;
  }
  throw Interpreter::error(name, "Undefined property '" + name->lexeme + "'.");
}

std::any Instance::set(SPToken name, std::any value) {
  fields[name->lexeme] = value;
  return value;
}
