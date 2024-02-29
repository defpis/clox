#include "test_util.h"
#include "lox.h"
#include "util.h"
#include <gtest/gtest.h>

namespace test_util {

void testProgram(const std::string &programPath, std::string expectedOutput, bool produceError) {
  std::string path = TEST_ROOT;
  path.append(programPath);

  if (produceError) {
    testing::internal::CaptureStderr();
  } else {
    testing::internal::CaptureStdout();
  }

  lox::runFile(path);

  std::string actualOutput;
  if (produceError) {
    actualOutput = testing::internal::GetCapturedStderr();
  } else {
    actualOutput = testing::internal::GetCapturedStdout();
  }

  expectedOutput = util::trimString(expectedOutput);
  actualOutput = util::trimString(actualOutput);

  ASSERT_EQ(expectedOutput, actualOutput);
}

} // namespace test_util
