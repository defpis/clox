#ifndef CLOX_TEST_UTIL_H
#define CLOX_TEST_UTIL_H

#include <string>

namespace test_util {

void testProgram(const std::string &programPath, std::string expectedOutput, bool produceError);

}

#endif // CLOX_TEST_UTIL_H
