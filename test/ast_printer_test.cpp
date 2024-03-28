#include "ast_printer.h"
#include "scanner.h"
#include "token.h"
#include <gtest/gtest.h>

TEST(ast_print_test, 1) {
  SPExpr expression = std::make_shared<BinaryExpr>(
      std::make_shared<UnaryExpr>(std::make_shared<Token>(TokenType::MINUS, "-", nullptr, 1),
                                  std::make_shared<LiteralExpr>(123)),
      std::make_shared<Token>(TokenType::STAR, "*", nullptr, 1),
      std::make_shared<GroupingExpr>(std::make_shared<LiteralExpr>(45.67)));

  ASSERT_EQ(AstPrinter().print(expression), "(* (- 123) (group 45.67))");
}

TEST(ast_print_test, 2) {
  auto fn = []() -> std::string {
    Scanner scanner;
    auto tokens = scanner.scanTokens("print 1;");

    std::string result;
    for (auto &token : tokens) {
      result.append(token->toString());
    }

    return result;
  };

  ASSERT_EQ(
      fn(),
      "Token<PRINT | print | nil | 1>Token<NUMBER | 1 | 1 | 1>Token<SEMICOLON | ; | nil | 1>Token<EOF |  | nil | 1>");
}
