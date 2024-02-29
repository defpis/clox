#include "test_util.h"
#include "lox.h"
#include "util.h"
#include <gtest/gtest.h>

void testProgram(const std::string &programPath, std::string expectedOutput, bool produceError) {
  std::string path = TEST_ROOT;
  path.append(programPath);

  if (produceError) {
    testing::internal::CaptureStderr();
  } else {
    testing::internal::CaptureStdout();
  }

  Lox::runFile(path);

  std::string actualOutput;
  if (produceError) {
    actualOutput = testing::internal::GetCapturedStderr();
  } else {
    actualOutput = testing::internal::GetCapturedStdout();
  }

  expectedOutput = trimString(expectedOutput);
  actualOutput = trimString(actualOutput);

  ASSERT_EQ(expectedOutput, actualOutput);
}
