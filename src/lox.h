#ifndef CLOX_LOX_H
#define CLOX_LOX_H

#include "interpreter.h"
#include "parser.h"
#include "scanner.h"
#include "token.h"
#include <string>

class Lox {
private:
  static bool _hadError;

  static Scanner _scanner;
  static Parser _parser;
  static Interpreter _interpreter;

public:
  static void runCmd(int argc, char **argv);

  static void runRepl();
  static void runFile(const std::string &path);
  static void runCode(const std::string &code);

  static void error(int line, const std::string &message);
  static void error(const SPToken &token, const std::string &message);

  static void report(int line, const std::string &where, const std::string &message);
};

#endif // CLOX_LOX_H
