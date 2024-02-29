#include "environment.h"
#include "interpreter.h"

void Environment::define(const std::string &name, const std::any &value) { values[name] = value; }

std::any Environment::get(SPToken name) { // NOLINT(*-no-recursion)
  auto it = values.find(name->lexeme);
  if (it != values.end()) {
    return it->second;
  }

  if (enclosing) {
    return enclosing->get(name);
  }

  throw Interpreter::error(name, "Undefined variable '" + name->lexeme + "'.");
}

std::any Environment::getAt(int distance, const std::string &name) { return ancestor(distance)->values.at(name); }

SPEnvironment Environment::ancestor(int distance) {
  SPEnvironment environment = shared_from_this();

  for (int i = 0; i < distance; i++) {
    environment = environment->enclosing;
  }

  return environment;
}

void Environment::assign(SPToken name, const std::any &value) { // NOLINT(*-no-recursion)
  auto it = values.find(name->lexeme);
  if (it != values.end()) {
    values[name->lexeme] = value;
    return;
  }

  if (enclosing) {
    enclosing->assign(name, value);
    return;
  }

  throw Interpreter::error(name, "Undefined variable '" + name->lexeme + "'.");
}

void Environment::assignAt(int distance, const std::string &name, const std::any &value) {
  ancestor(distance)->values[name] = value;
}
