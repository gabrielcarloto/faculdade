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

    it("should remove an index", [&]() {
      const size_t indexToRemove = LIST_LENGTH - 10;
      const int indexValue = vec.at(indexToRemove);
      const size_t expectedLength = LIST_LENGTH - 1;
      const int predecessorValue = vec.at(indexToRemove - 1);
      const int successorValue = vec.at(indexToRemove + 1);

      vec.remove(indexToRemove);
      assert(vec.at(indexToRemove) != indexValue);
      assert(vec.at(indexToRemove) == successorValue);
      assert(vec.at(indexToRemove - 1) == predecessorValue);
    });

    // NOTE: currently, each tests depends on the previous tests
    // so the expected length will be LIST_LENGTH.
    // This isn't ideal, but it is better than not testing
    it("should add an item", [&]() {
      const int valueToInsert = 50;
      const size_t indexToInsert = 10;
      const int indexValue = indexToInsert;
      const size_t expectedLength = LIST_LENGTH;
      const int predecessorValue = indexToInsert - 1;
      const int successorValue = indexToInsert + 1;

      assert(vec.at(indexToInsert) == indexValue);
      vec.insert(valueToInsert, indexToInsert);

      assert(vec.at(indexToInsert) == valueToInsert);
      assert(vec.at(indexToInsert + 1) == indexValue);
      assert(vec.at(indexToInsert + 2) == successorValue);
      assert(vec.at(indexToInsert - 1) == predecessorValue);
      assert(vec.getLength() == expectedLength);
    });

    it("should replace an item", [&]() {
      const int valueToReplace = 50;
      const size_t indexToReplace = 5;
      const int indexValue = indexToReplace;
      const size_t expectedLength = LIST_LENGTH;
      const int predecessorValue = indexToReplace - 1;
      const int successorValue = indexToReplace + 1;

      assert(vec.at(indexToReplace) == indexValue);
      vec.replace(valueToReplace, indexToReplace);

      assert(vec.at(indexToReplace) == valueToReplace);
      assert(vec.at(indexToReplace + 1) == successorValue);
      assert(vec.at(indexToReplace - 1) == predecessorValue);
      assert(vec.getLength() == expectedLength);
    });
  });

  return 0;
}

template <typename Derived> void mockList(BaseList<int, Derived> &list) {
  for (size_t i = 0; i < LIST_LENGTH; i++) {
    list.push(i);
  }
}
