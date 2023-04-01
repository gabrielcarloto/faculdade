#pragma once
#include <functional>
#include <iostream>
#include <stdexcept>

using TestCallback = const std::function<void()> &;

#define TEST_SUCCESS "\033[32mPASS\033[39m"
#define TEST_ERROR "\033[31mFAIL\033[39m"

#define assert(condition)                                                      \
  if (!(condition))                                                            \
  throw std::logic_error("Assertion failed: " #condition)

namespace test {
void describe(std::string, TestCallback);
void it(std::string, TestCallback);
} // namespace test