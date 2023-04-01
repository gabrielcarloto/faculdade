#include "BaseList.h"
#include "List.h"
#include "Vector.h"
#include "test.h"
#include <stdexcept>
#include <stdint.h>

using namespace test;

#define LIST_LENGTH 1000

template <typename Derived> void mockList(BaseList<int, Derived> &list);

int main() {
  describe("Vector", []() {
    Vector<int> vec;

    it("should reserve the the correct capacity", [&]() {
      vec.reserve(LIST_LENGTH);
      expectEqual(vec.getCapacity(), LIST_LENGTH);
    });

    it("should add items without errors", [&]() { mockList(vec); });

    it("should have LIST_LENGTH length",
       [&]() { expectEqual(vec.getLength(), LIST_LENGTH); });

    it("should return the correct items using the [] operator", [&]() {
      const size_t index = 7;
      const int expectedResult = index;

      expectEqual(vec[index], expectedResult);
    });

    it("should return the correct items using the `at` method", [&]() {
      const size_t unsignedIndex = 7;
      const intmax_t signedIndex = -2;
      const int unsignedIndexExpectedResult = unsignedIndex;
      const int signedIndexExpectedResult = LIST_LENGTH + signedIndex;

      expectEqual(vec.at(unsignedIndex), unsignedIndexExpectedResult);
      expectEqual(vec.at(signedIndex), signedIndexExpectedResult);
    });

    it("should throw when trying to access index out of range", [&]() {
      const size_t unsignedIndex = LIST_LENGTH;
      const intmax_t signedIndex = -LIST_LENGTH;

      expectThrow(vec.at, unsignedIndex);
      expectThrow(vec.at, signedIndex);
    });

    it("should loop through every item using the `forEach` method",
       [&]() { vec.forEach([](auto item, auto i) { expectEqual(item, i); }); });

    it("should find an item", [&]() {
      const int itemToFind = LIST_LENGTH / 2;
      int item = -1;

      bool found =
          vec.find([](auto item, auto _i) { return item == itemToFind; }, item);

      expectEqual(found, true);
      expectEqual(item, itemToFind);
    });

    it("should find the index of an item", [&]() {
      const int itemToFind = LIST_LENGTH / 2;
      const size_t expectedIndex = itemToFind;
      size_t item = -1;

      bool found = vec.findIndex(
          [](auto item, auto _i) { return item == itemToFind; }, item);

      expectEqual(found, true);
      expectEqual(item, expectedIndex);
    });

    it("should remove an index", [&]() {
      const size_t indexToRemove = LIST_LENGTH - 10;
      const int indexValue = vec.at(indexToRemove);
      const size_t expectedLength = LIST_LENGTH - 1;
      const int predecessorValue = vec.at(indexToRemove - 1);
      const int successorValue = vec.at(indexToRemove + 1);

      vec.remove(indexToRemove);
      expectDifer(vec.at(indexToRemove), indexValue);
      expectEqual(vec.at(indexToRemove), successorValue);
      expectEqual(vec.at(indexToRemove - 1), predecessorValue);
    });

    it("should shrink to fit", [&]() {
      const size_t capacityBeforeShrinking = vec.getCapacity();

      vec.shrinkToFit();

      expectGreaterThan(capacityBeforeShrinking, vec.getCapacity());
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

      expectEqual(vec.at(indexToInsert), indexValue);
      vec.insert(valueToInsert, indexToInsert);

      expectEqual(vec.at(indexToInsert), valueToInsert);
      expectEqual(vec.at(indexToInsert + 1), indexValue);
      expectEqual(vec.at(indexToInsert + 2), successorValue);
      expectEqual(vec.at(indexToInsert - 1), predecessorValue);
      expectEqual(vec.getLength(), expectedLength);
    });

    it("should replace an item", [&]() {
      const int valueToReplace = 50;
      const size_t indexToReplace = 5;
      const int indexValue = indexToReplace;
      const size_t expectedLength = LIST_LENGTH;
      const int predecessorValue = indexToReplace - 1;
      const int successorValue = indexToReplace + 1;

      expectEqual(vec.at(indexToReplace), indexValue);
      vec.replace(valueToReplace, indexToReplace);

      expectEqual(vec.at(indexToReplace), valueToReplace);
      expectEqual(vec.at(indexToReplace + 1), successorValue);
      expectEqual(vec.at(indexToReplace - 1), predecessorValue);
      expectEqual(vec.getLength(), expectedLength);
    });
  });

  return 0;
}

template <typename Derived> void mockList(BaseList<int, Derived> &list) {
  for (size_t i = 0; i < LIST_LENGTH; i++) {
    list.push(i);
  }
}
