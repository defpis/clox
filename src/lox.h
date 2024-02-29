#ifndef CLOX_LOX_H
#define CLOX_LOX_H

#include "interpreter.h"
#include "parser.h"
#include "resolver.h"
#include "scanner.h"
#include "token.h"
#include <string>

namespace lox {

static bool hadError;
static bool hadWarn;

// 使用静态变量作为单例有很严重问题，如果类成员属性中包含静态属性，那么初始化顺序可能无法确定，会导致多次初始化
// static Scanner scanner;
// static Parser parser;
// static Interpreter interpreter;
// static Resolver resolver;

void runCmd(int argc, char **argv);
void runRepl();
void runFile(const std::string &path);
void runCode(const std::string &code);

void error(int line, const std::string &message);
void error(SPToken token, const std::string &message);

void warn(int line, const std::string &message);
void warn(SPToken token, const std::string &message);

void report(int line, const std::string &where, const std::string &message);

} // namespace lox

#endif // CLOX_LOX_H
