#pragma once
#include "utils.h"
#include <functional>
#include <iostream>
#include <sstream>
#include <stdexcept>

using TestCallback = const std::function<void()> &;

#define assert(condition)                                                      \
  if (!(condition))                                                            \
  throw std::logic_error("Assertion failed: " #condition)

#define expectEqual(lhs, rhs)                                                  \
  if (lhs != rhs) {                                                            \
    std::ostringstream stringStream;                                           \
    stringStream << "Expected " << lhs << " (exp: " #lhs ") to be equal to "   \
                 << rhs << " (exp: " #rhs ")\n";                               \
    throw std::logic_error(stringStream.str());                                \
  }

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
