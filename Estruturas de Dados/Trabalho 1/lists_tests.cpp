#include "BaseList.h"
#include "List.h"
#include "Vector.h"
#include "test.h"
#include <stdint.h>

using namespace test;

#define LIST_LENGTH 1000

template <typename Derived> void mockList(BaseList<int, Derived> &list);

int main() {
  describe("Vector", []() {
    Vector<int> vec;

    it("should reserve the the correct capacity", [&]() {
      vec.reserve(LIST_LENGTH);
      assert(vec.getCapacity() == LIST_LENGTH);
    });

    it("should add items without errors", [&]() { mockList(vec); });

    it("should have LIST_LENGTH length",
       [&]() { assert(vec.getLength() == LIST_LENGTH); });

    it("should return the correct items using the [] operator", [&]() {
      const size_t index = 7;
      const int expectedResult = index;

      assert(vec[index] == expectedResult);
    });

    it("should return the correct items using the `at` method", [&]() {
      const size_t unsignedIndex = 7;
      const intmax_t signedIndex = -2;
      const int unsignedIndexExpectedResult = unsignedIndex;
      const int signedIndexExpectedResult = LIST_LENGTH + signedIndex;

      assert(vec.at(unsignedIndex) == unsignedIndexExpectedResult);
      assert(vec.at(signedIndex) == signedIndexExpectedResult);
    });

    it("should throw when trying to access index out of range", [&]() {
      const size_t unsignedIndex = LIST_LENGTH;
      const intmax_t signedIndex = -LIST_LENGTH;

      expectThrow(vec.at, unsignedIndex);
      expectThrow(vec.at, signedIndex);
    });
  });

  return 0;
}

template <typename Derived> void mockList(BaseList<int, Derived> &list) {
  for (size_t i = 0; i < LIST_LENGTH; i++) {
    list.push(i);
  }
}
