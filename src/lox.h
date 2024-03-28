#ifndef CLOX_LOX_H
#define CLOX_LOX_H

#include "interpreter.h"
#include "parser.h"
#include "scanner.h"
#include "token.h"
#include <string>

namespace lox {

static bool _hadError;

static Scanner _scanner;
static Parser _parser;
static Interpreter _interpreter;

void runCmd(int argc, char **argv);
void runRepl();
void runFile(const std::string &path);
void runCode(const std::string &code);
void error(int line, const std::string &message);
void error(const SPToken &token, const std::string &message);
void report(int line, const std::string &where, const std::string &message);

} // namespace lox

#endif // CLOX_LOX_H
