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
    stringStream << "Expected " << lhs << " (" #lhs ") to equal " << rhs       \
                 << " (" #rhs ")\n";                                           \
    throw std::logic_error(stringStream.str());                                \
  }

#define expectDifer(lhs, rhs)                                                  \
  if (lhs == rhs) {                                                            \
    std::ostringstream stringStream;                                           \
    stringStream << "Expected " << lhs << " (" #lhs ") to not equal " << rhs   \
                 << " (" #rhs ")\n";                                           \
    throw std::logic_error(stringStream.str());                                \
  }

#define expectGreaterThan(lhs, rhs)                                            \
  if (lhs <= rhs) {                                                            \
    std::ostringstream stringStream;                                           \
    stringStream << "Expected " << lhs << " (" #lhs ") to be greater than "    \
                 << rhs << " (" #rhs ")\n";                                    \
    throw std::logic_error(stringStream.str());                                \
  }

#define expectLessThan(lhs, rhs)                                               \
  if (lhs >= rhs) {                                                            \
    std::ostringstream stringStream;                                           \
    stringStream << "Expected " << lhs << " (" #lhs ") to be less than "       \
                 << rhs << " (" #rhs ")\n";                                    \
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
