#include "lox.h"
#include "ast_printer.h"
#include "linenoise/linenoise.h"
#include "util.h"
#include <iostream>

using namespace util;

namespace lox {

void runCmd(int argc, char **argv) {
  if (argc > 2) {
    std::cout << "Usage: lox[ script]" << std::endl;
    std::exit(64);
  } else if (argc == 2) {
    runFile(argv[1]);
  } else {
    runRepl();
  }
}

// TODO: REPL支持多行输入和编辑

// 使用 linenoise 需要在 Clion 中开启 `Emulate terminal in the output console`

void runRepl() {
  // 允许多行显示
  linenoiseSetMultiLine(1);

  std::string line;

  while (true) {
    line = linenoise("lox> ");
    line = trimString(line);

    // 空行跳过
    if (line.empty()) {
      continue;
    }

    // 记录输入命令
    linenoiseHistoryAdd(line.c_str());

    // 手动输入命令退出
    if (line == "exit") {
      break;
    }

    // 清空控制台消息
    if (line == "clear") {
      linenoiseClearScreen();
      continue;
    }

    runCode(line);
  }
}

void runFile(const std::string &path) {
  std::string source = readFile(path);
  runCode(source);
  if (_hadError) {
    std::exit(65);
  }
}

void runCode(const std::string &code) {
  auto tokens = _scanner.scanTokens(code);

  for (auto &token : tokens) {
    std::cout << token->toString() << std::endl;
  }

  auto statements = _parser.parse(tokens);

  _interpreter.interpret(statements);
}

void error(int line, const std::string &message) { report(line, "", message); }

void error(const SPToken &token, const std::string &message) {
  if (token->type == TokenType::EOF_) {
    report(token->line, " at end", message);
  } else {
    report(token->line, " at '" + token->lexeme + "'", message);
  }
}

void report(int line, const std::string &where, const std::string &message) {
  std::cout << "[line " << line << "] Error" << where << ": " << message << std::endl;
  _hadError = true;
}

} // namespace lox
