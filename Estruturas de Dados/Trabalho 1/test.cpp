#include "test.h"
#include <exception>

void test::describe(std::string description, TestCallback fn) {
  std::cout << description << "\n";
  fn();
}

void test::it(std::string description, TestCallback fn) {
  try {
    fn();
    std::cout << "    " << TEST_SUCCESS << " " << description << "\n";
  } catch (...) {
    std::exception_ptr e = std::current_exception();

    std::cout << "    " << TEST_ERROR << " " << description << "\n\n";
    std::rethrow_exception(e);
  }
}
