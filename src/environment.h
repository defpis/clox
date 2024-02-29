#ifndef CLOX_ENVIRONMENT_H
#define CLOX_ENVIRONMENT_H

#include "token.h"
#include <any>
#include <map>

class Environment;

using SPEnvironment = std::shared_ptr<Environment>;

class Environment : public std::enable_shared_from_this<Environment> {
private:
  std::map<std::string, std::any> values;
  int depth = 0;
  SPEnvironment enclosing;

public:
  Environment() = default;
  explicit Environment(SPEnvironment enclosing) : depth(enclosing->depth + 1), enclosing(std::move(enclosing)) {}

  void define(const std::string &name, const std::any &value);
  std::any get(SPToken name);
  std::any getAt(int distance, const std::string &name);
  SPEnvironment ancestor(int distance);
  void assign(SPToken name, const std::any &value);
  void assignAt(int distance, const std::string &name, const std::any &value);
};

#endif // CLOX_ENVIRONMENT_H
