#include "BaseList.h"
#include "Vector.h"
#include "test.h"

using namespace test;

#define LIST_LENGTH 1000

void mockList(BaseList<int> &list);

int main() {
  describe("Vector", []() {
    Vector<int> vec;
    mockList(vec);

    it("should have LIST_LENGTH length",
       [&]() { assert(vec.getLength() == LIST_LENGTH); });

    it("should return the correct items by index", [&]() {
      const size_t unsignedIndex = 7;
      const intmax_t signedIndex = -2;
      const int unsignedIndexExpectedResult = unsignedIndex;
      const int signedIndexExpectedResult = LIST_LENGTH + signedIndex;

      assert(vec[unsignedIndex] == unsignedIndexExpectedResult);
      assert(vec.at(unsignedIndex) == unsignedIndexExpectedResult);
      assert(vec.at(signedIndex) == signedIndexExpectedResult);
    });
  });

  return 0;
}

void mockList(BaseList<int> &list) {
  for (size_t i = 0; i < LIST_LENGTH; i++) {
    list.push(i);
  }
}
