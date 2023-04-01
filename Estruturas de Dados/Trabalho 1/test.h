#pragma once
#include <functional>
#include <iostream>
#include <stdexcept>

using TestCallback = const std::function<void()> &;

#define assert(condition)                                                      \
  if (!(condition))                                                            \
  throw std::logic_error("Assertion failed: " #condition)

#define expectThrow(fn, ...)                                                   \
  try {                                                                        \
    fn(__VA_ARGS__);                                                           \
    throw std::runtime_error("Function call expected to throw but did not");   \
  } catch (...) {                                                              \
  }

namespace test {
void describe(std::string, TestCallback);
void it(std::string, TestCallback);
} // namespace test
