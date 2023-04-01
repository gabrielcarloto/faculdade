#include "BaseList.h"
#include "Vector.h"
#include "test.h"
#include <exception>

using namespace test;

#define LIST_LENGTH 1000

void mockList(BaseList<int> &list);

int main() {
  describe("Vector", []() {
    Vector<int> vec;
    mockList(vec);

    it("should have LIST_LENGTH length",
       [&]() { assert(vec.getLength() == LIST_LENGTH); });
  });

  return 0;
}

void mockList(BaseList<int> &list) {
  for (size_t i = 0; i < LIST_LENGTH; i++) {
    list.push(i);
  }
}
