#include "ast_printer.h"
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
