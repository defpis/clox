#include "lox.h"
#include "ast_printer.h"
#include "linenoise/linenoise.h"
#include "util.h"
#include <iostream>

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
    line = util::trimString(line);

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
  std::string source = util::readFile(path);
  runCode(source);
  if (hadError) {
    std::exit(65);
  }
}

void runCode(const std::string &code) {
  Scanner &scanner = Scanner::getInstance();
  std::vector<SPToken> tokens = scanner.scanTokens(code);

  Parser &parser = Parser::getInstance();
  std::vector<SPStmt> statements = parser.parse(tokens);

  Resolver &resolver = Resolver::getInstance();
  std::map<SPExpr, int> locals = resolver.resolve(statements);

  Interpreter &interpreter = Interpreter::getInstance();
  interpreter.interpret(statements, locals);
}

void error(int line, const std::string &message) {
  report(line, "Error", message);
  hadError = true;
}

void error(SPToken token, const std::string &message) {
  if (token->type == TokenType::EOF_) {
    report(token->line, "Error at end", message);
  } else {
    report(token->line, "Error at '" + token->lexeme + "'", message);
  }
  hadError = true;
}

void warn(int line, const std::string &message) {
  report(line, "Warn", message);
  hadWarn = true;
}

void warn(SPToken token, const std::string &message) {
  if (token->type == TokenType::EOF_) {
    report(token->line, "Warn at end", message);
  } else {
    report(token->line, "Warn at '" + token->lexeme + "'", message);
  }
  hadWarn = true;
}

void report(int line, const std::string &where, const std::string &message) {
  std::cerr << "[line " << line << "] " << where << ": " << message << std::endl;
}

} // namespace lox
