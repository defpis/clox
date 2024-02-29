#include "util.h"
#include <fstream>
#include <sstream>

namespace util {

std::string readFile(const std::string &path) {
  std::ifstream file(path);
  std::stringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}

std::string trimString(std::string string) { return trimString(std::move(string), " \n\r\t"); }

std::string trimString(std::string string, const std::string &trimChars) {
  string.erase(0, string.find_first_not_of(trimChars)); // left
  string.erase(string.find_last_not_of(trimChars) + 1); // right
  return string;
}

std::string trimNumberString(std::string string) {
  string.erase(string.find_last_not_of('0') + 1); // 移除多余的零
  if (string.back() == '.') {
    string.pop_back(); // 移除小数点
  }
  return string;
}

std::string joinString(const std::vector<std::string> &list) { return joinString(list, ","); }

std::string joinString(const std::vector<std::string> &list, const std::string &delimiter) {
  size_t last = list.size() - 1;

  std::string string;

  for (int i = 0; i <= last; i++) {
    if (!list.at(i).empty()) {
      string += list.at(i);
    }
    if (i != last) {
      string += delimiter;
    }
  }

  return string;
}

std::optional<std::string> toString(const std::any &value) {
  if (!value.has_value() || isNull(value)) {
    return "nil";
  }

  if (isString(value)) {
    return std::any_cast<std::string>(value);
  }
  if (isInt(value)) {
    return std::to_string(std::any_cast<int>(value));
  }
  if (isLong(value)) {
    return std::to_string(std::any_cast<long>(value));
  }
  if (isLongLong(value)) {
    return std::to_string(std::any_cast<long long>(value));
  }
  if (isFloat(value)) {
    return trimNumberString(std::to_string(std::any_cast<float>(value))); // 去除末尾的'.'或'0'
  }
  if (isDouble(value)) {
    return trimNumberString(std::to_string(std::any_cast<double>(value))); // 去除末尾的'.'或'0'
  }
  if (isBool(value)) {
    return std::any_cast<bool>(value) ? "true" : "false";
  }

  return std::nullopt;
}

std::string toString(const std::any &value, const std::string &defaultValue) {
  std::optional<std::string> optString = toString(value);

  if (optString.has_value()) {
    return optString.value();
  }

  return defaultValue;
}

std::optional<double> toNumber(const std::any &value) {
  if (!value.has_value() || isNull(value)) {
    return 0;
  }

  if (isString(value)) {
    auto [ok, number] = stringToNumber(std::any_cast<std::string>(value));
    if (ok) {
      return number;
    }
  }
  if (isInt(value)) {
    return std::any_cast<int>(value);
  }
  if (isLong(value)) {
    return std::any_cast<long>(value);
  }
  if (isLongLong(value)) {
    return std::any_cast<long long>(value);
  }
  if (isFloat(value)) {
    return std::any_cast<float>(value);
  }
  if (isDouble(value)) {
    return std::any_cast<double>(value);
  }
  if (isBool(value)) {
    return std::any_cast<bool>(value) ? 1 : 0;
  }

  return std::nullopt;
}

double toNumber(const std::any &value, double defaultValue) {
  std::optional<double> optNumber = toNumber(value);

  if (optNumber.has_value()) {
    return optNumber.value();
  }

  return defaultValue;
}

std::optional<bool> toBool(const std::any &value) {
  if (!value.has_value() || isNull(value)) {
    return false;
  }

  if (isString(value)) {
    return !std::any_cast<std::string>(value).empty();
  }
  if (isInt(value)) {
    return std::any_cast<int>(value) != 0;
  }
  if (isLong(value)) {
    return std::any_cast<long>(value) != 0;
  }
  if (isLongLong(value)) {
    return std::any_cast<long long>(value) != 0;
  }
  if (isFloat(value)) {
    return std::any_cast<float>(value) != 0;
  }
  if (isDouble(value)) {
    return std::any_cast<double>(value) != 0;
  }
  if (isBool(value)) {
    return std::any_cast<bool>(value);
  }

  return std::nullopt;
}

bool toBool(const std::any &value, bool defaultValue) {
  std::optional<bool> optBool = toBool(value);

  if (optBool.has_value()) {
    return optBool.value();
  }

  return defaultValue;
}

bool isString(const std::any &value) {
  auto &type = value.type();

  if (type == typeid(std::string)) {
    return true;
  }

  return false;
}

bool isInt(const std::any &value) {
  auto &type = value.type();

  if (type == typeid(int)) {
    return true;
  }

  return false;
}

bool isLong(const std::any &value) {
  auto &type = value.type();

  if (type == typeid(long)) {
    return true;
  }

  return false;
}

bool isLongLong(const std::any &value) {
  auto &type = value.type();

  if (type == typeid(long long)) {
    return true;
  }

  return false;
}

bool isFloat(const std::any &value) {
  auto &type = value.type();

  if (type == typeid(float)) {
    return true;
  }

  return false;
}

bool isDouble(const std::any &value) {
  auto &type = value.type();

  if (type == typeid(double)) {
    return true;
  }

  return false;
}

bool isNumber(const std::any &value) {
  if (isInt(value) || isDouble(value) || isLong(value) || isLongLong(value)) {
    return true;
  }

  return false;
}

bool isBool(const std::any &value) {
  auto &type = value.type();

  if (type == typeid(bool)) {
    return true;
  }

  return false;
}

bool isNull(const std::any &value) {
  auto &type = value.type();

  if (type == typeid(nullptr)) {
    return true;
  }

  return false;
}

bool isEqual(const std::any &a, const std::any &b) {
  if ((isString(a) || isNumber(a) || isBool(a)) && (isString(b) || isNumber(b) || isBool(b))) {
    return (toString(a) == toString(b)) || (toNumber(a) == toNumber(b)) || (toBool(a) == toBool(b));
  }
  return false;
}

std::pair<bool, double> stringToNumber(const std::string &value) {
  try {
    double number = std::stod(value);
    return std::make_pair(true, number);
  } catch (const std::invalid_argument &err) {
    return std::make_pair(false, 0);
  } catch (const std::out_of_range &err) {
    return std::make_pair(false, 0);
  }
}

} // namespace util
