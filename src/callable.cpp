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

std::size_t Clock::arity() { return 0; }

std::any Clock::call(Interpreter *interpreter, std::vector<std::any> &arguments) {
  auto now = std::chrono::system_clock::now();
  return std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
}

int Count::count = 0;

std::size_t Count::arity() { return 0; }

std::any Count::call(Interpreter *interpreter, std::vector<std::any> &arguments) { return ++count; }
