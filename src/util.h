#ifndef CLOX_UTIL_H
#define CLOX_UTIL_H

#include <any>
#include <optional>
#include <vector>

namespace util {

std::string readFile(const std::string &path);

std::string trimString(std::string string);

std::string trimString(std::string string, const std::string &trimChars);

std::string trimNumberString(std::string string);

std::string joinString(const std::vector<std::string> &list);

std::string joinString(const std::vector<std::string> &list, const std::string &delimiter);

std::optional<std::string> toString(const std::any &value);

std::string toString(const std::any &value, const std::string &defaultValue);

std::optional<double> toNumber(const std::any &value);

double toNumber(const std::any &value, double defaultValue);

std::optional<bool> toBool(const std::any &value);

bool toBool(const std::any &value, bool defaultValue);

bool isString(const std::any &value);

bool isInt(const std::any &value);

bool isLong(const std::any &value);

bool isLongLong(const std::any &value);

bool isFloat(const std::any &value);

bool isDouble(const std::any &value);

bool isNumber(const std::any &value);

bool isBool(const std::any &value);

bool isNull(const std::any &value);

bool isEqual(const std::any &a, const std::any &b);

std::pair<bool, double> stringToNumber(const std::string &value);

} // namespace util

#endif // CLOX_UTIL_H
