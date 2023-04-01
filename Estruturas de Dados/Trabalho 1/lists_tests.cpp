#include "BaseList.h"
#include "List.h"
#include "Vector.h"
#include "test.h"
#include <stdexcept>
#include <stdint.h>

using namespace test;

#define LIST_LENGTH 1000

template <typename Derived> void mockList(BaseList<int, Derived> &list);
template <typename Derived>
void testAccessOperator(BaseList<int, Derived> &list);
template <typename Derived> void testAtMethod(BaseList<int, Derived> &list);
template <typename Derived> void testOutOfRange(BaseList<int, Derived> &list);
template <typename Derived> void testForEach(BaseList<int, Derived> &list);
template <typename Derived> void testFind(BaseList<int, Derived> &list);
template <typename Derived> void testFindIndex(BaseList<int, Derived> &list);
template <typename Derived> void testRemove(BaseList<int, Derived> &list);
template <typename Derived> void testInsert(BaseList<int, Derived> &list);
template <typename Derived> void testReplace(BaseList<int, Derived> &list);

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

    it("should return the correct items using the [] operator",
       [&]() { testAccessOperator(vec); });

    it("should return the correct items using the `at` method",
       [&]() { testAtMethod(vec); });

    it("should throw when trying to access index out of range",
       [&]() { testOutOfRange(vec); });

    it("should loop through every item using the `forEach` method",
       [&]() { testForEach(vec); });

    it("should find an item", [&]() { testFind(vec); });

    it("should find the index of an item", [&]() { testFindIndex(vec); });

    it("should remove an index", [&]() { testRemove(vec); });

    it("should shrink to fit", [&]() {
      const size_t capacityBeforeShrinking = vec.getCapacity();

      vec.shrinkToFit();

      expectGreaterThan(capacityBeforeShrinking, vec.getCapacity());
    });

    it("should insert an item", [&]() { testInsert(vec); });

    it("should replace an item", [&]() { testReplace(vec); });
  });

  describe("List", []() {
    List<int> list;

    it("should add items without errors", [&]() { mockList(list); });

    it("should have LIST_LENGTH length",
       [&]() { expectEqual(list.getLength(), LIST_LENGTH); });

    it("should return the correct items using the [] operator",
       [&]() { testAccessOperator(list); });

    it("should return the correct items using the `at` method",
       [&]() { testAtMethod(list); });

    it("should throw when trying to access index out of range",
       [&]() { testOutOfRange(list); });

    it("should loop through every item using the `forEach` method",
       [&]() { testForEach(list); });

    it("should find an item", [&]() { testFind(list); });

    it("should find the index of an item", [&]() { testFindIndex(list); });

    it("should remove an index", [&]() { testRemove(list); });

    it("should insert an item", [&]() { testInsert(list); });

    it("should replace an item", [&]() { testReplace(list); });
  });

  return 0;
}

template <typename Derived> void mockList(BaseList<int, Derived> &list) {
  for (size_t i = 0; i < LIST_LENGTH; i++) {
    list.push(i);
  }
}

template <typename Derived>
void testAccessOperator(BaseList<int, Derived> &list) {
  const size_t index = 7;
  const int expectedResult = index;

  expectEqual(list[index], expectedResult);
}

template <typename Derived> void testAtMethod(BaseList<int, Derived> &list) {
  const size_t unsignedIndex = 7;
  const intmax_t signedIndex = -2;
  const int unsignedIndexExpectedResult = unsignedIndex;
  const int signedIndexExpectedResult = LIST_LENGTH + signedIndex;

  expectEqual(list.at(unsignedIndex), unsignedIndexExpectedResult);
  expectEqual(list.at(signedIndex), signedIndexExpectedResult);
}

template <typename Derived> void testOutOfRange(BaseList<int, Derived> &list) {
  const size_t unsignedIndex = LIST_LENGTH;
  const intmax_t signedIndex = -LIST_LENGTH;

  expectThrow(list.at, unsignedIndex);
  expectThrow(list.at, signedIndex);
}

template <typename Derived> void testForEach(BaseList<int, Derived> &list) {
  list.forEach(
      [](auto item, auto i) { expectEqual(static_cast<size_t>(item), i); });
}

template <typename Derived> void testFind(BaseList<int, Derived> &list) {
  const int itemToFind = LIST_LENGTH / 2;
  int item = -1;

  bool found =
      list.find([](auto item, auto) { return item == itemToFind; }, item);

  expectEqual(found, true);
  expectEqual(item, itemToFind);
}

template <typename Derived> void testFindIndex(BaseList<int, Derived> &list) {
  const int itemToFind = LIST_LENGTH / 2;
  const size_t expectedIndex = itemToFind;
  size_t item = -1;

  bool found =
      list.findIndex([](auto item, auto) { return item == itemToFind; }, item);

  expectEqual(found, true);
  expectEqual(item, expectedIndex);
}

template <typename Derived> void testRemove(BaseList<int, Derived> &list) {
  const size_t indexToRemove = LIST_LENGTH - 10;
  const int indexValue = list.at(indexToRemove);
  const size_t expectedLength = LIST_LENGTH - 1;
  const int predecessorValue = list.at(indexToRemove - 1);
  const int successorValue = list.at(indexToRemove + 1);

  list.remove(indexToRemove);
  expectEqual(list.getLength(), expectedLength);
  expectDifer(list.at(indexToRemove), indexValue);
  expectEqual(list.at(indexToRemove), successorValue);
  expectEqual(list.at(indexToRemove - 1), predecessorValue);
}

// NOTE: currently, each test depends on the previous tests
// so the expected length will be LIST_LENGTH.
// This isn't ideal, but it is better than not testing
template <typename Derived> void testInsert(BaseList<int, Derived> &list) {
  const int valueToInsert = 50;
  const size_t indexToInsert = 10;
  const int indexValue = indexToInsert;
  const size_t expectedLength = LIST_LENGTH;
  const int predecessorValue = indexToInsert - 1;
  const int successorValue = indexToInsert + 1;

  expectEqual(list.at(indexToInsert), indexValue);
  list.insert(valueToInsert, indexToInsert);

  expectEqual(list.at(indexToInsert), valueToInsert);
  expectEqual(list.at(indexToInsert + 1), indexValue);
  expectEqual(list.at(indexToInsert + 2), successorValue);
  expectEqual(list.at(indexToInsert - 1), predecessorValue);
  expectEqual(list.getLength(), expectedLength);
}

template <typename Derived> void testReplace(BaseList<int, Derived> &list) {
  const int valueToReplace = 50;
  const size_t indexToReplace = 5;
  const int indexValue = indexToReplace;
  const size_t expectedLength = LIST_LENGTH;
  const int predecessorValue = indexToReplace - 1;
  const int successorValue = indexToReplace + 1;

  expectEqual(list.at(indexToReplace), indexValue);
  list.replace(valueToReplace, indexToReplace);

  expectEqual(list.at(indexToReplace), valueToReplace);
  expectEqual(list.at(indexToReplace + 1), successorValue);
  expectEqual(list.at(indexToReplace - 1), predecessorValue);
  expectEqual(list.getLength(), expectedLength);
}
