#include "expr.h"

std::any intToDoubleIfNeed(const std::any &value) {
  if (value.type() == typeid(int)) {
    return static_cast<double>(std::any_cast<int>(value));
  }
  return value;
}

LiteralExpr::LiteralExpr(const std::any &val) { value = intToDoubleIfNeed(val); }
