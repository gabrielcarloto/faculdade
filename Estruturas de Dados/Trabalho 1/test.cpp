#include "test.h"
#include <exception>
#include <string.h>

#define BG_GREEN "\033[30;42m"
#define BG_RED "\033[30;41m"
#define BG_YELLOW "\033[30;43m"
#define RESET_COLOR "\033[39;49m"

#define TEST_SUCCESS BG_GREEN " PASS " RESET_COLOR
#define TEST_SUITE_TEXT " TEST "
#define TEST_SUITE BG_YELLOW TEST_SUITE_TEXT RESET_COLOR
#define TEST_ERROR BG_RED " FAIL " RESET_COLOR

void test::describe(std::string description, TestCallback fn) {
  std::cout << TEST_SUITE << " " << description << "\n";
  fn();
}

void test::it(std::string description, TestCallback fn) {
  const int testSuiteLength = strlen(TEST_SUITE_TEXT);
  const std::string tab = std::string(testSuiteLength, ' ');

  try {
    fn();
    std::cout << tab << TEST_SUCCESS << " " << description << "\n";
  } catch (...) {
    std::exception_ptr e = std::current_exception();

    std::cout << tab << TEST_ERROR << " " << description << "\n\n";
    std::rethrow_exception(e);
  }
}
