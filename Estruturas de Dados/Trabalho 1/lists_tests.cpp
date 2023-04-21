#define _DEBUG
#include "BaseList.h"
#include "List.h"
#include "Vector.h"
#include "test.h"
#include <cstdint>
#include <stdexcept>
#include <type_traits>
#include <vector>

using namespace test;

enum { LIST_LENGTH = 1000 };

template <class Derived> class TestBaseListDerivedClass {
  using ListType = Derived &;

  Derived beforeEachTest() { return initBaseListDerivedClass(); };

  void testAccessOperator(ListType list) {
    it("should return the correct items using the [] operator", [&]() {
      const size_t index = 7;
      const int expectedResult = index;

      expectEqual(list[index], expectedResult);
    });
  }

  void testAtMethodWithPositiveAndNegativeIndices(ListType list) {
    it("should return the correct items using the `at` method", [&]() {
      const size_t unsignedIndex = 7;
      const size_t unsignedIndex2 = 0;
      const intmax_t signedIndex = -2;
      const intmax_t signedIndex2 = -1;
      const int unsignedIndexExpectedResult = unsignedIndex;
      const int unsignedIndexExpectedResult2 = unsignedIndex2;
      const int signedIndexExpectedResult = LIST_LENGTH + signedIndex;
      const int signedIndexExpectedResult2 = LIST_LENGTH + signedIndex2;

      const int atUnsigned = list.at(unsignedIndex),
                atUnsigned2 = list.at(unsignedIndex2);

      expectEqual(list.at(signedIndex), signedIndexExpectedResult);
      expectEqual(list.at(signedIndex2), signedIndexExpectedResult2);
      expectEqual(atUnsigned2, unsignedIndexExpectedResult2);
      expectEqual(atUnsigned, unsignedIndexExpectedResult);
      expectEqual(atUnsigned2, list[unsignedIndex2]);
      expectEqual(atUnsigned, list[unsignedIndex]);
    });
  }

  void testOutOfRangeThrow(ListType list) {
    it("should throw when trying to access index out of range", [&]() {
      const size_t unsignedIndex = LIST_LENGTH;
      const intmax_t signedIndex = -LIST_LENGTH;
      const auto lambda = [](auto, auto) {};
      const int item = 10;

      expectThrow(list.at, unsignedIndex);
      expectThrow(list.at, signedIndex);
      expectThrow(list.remove, signedIndex);
      expectThrow(list.insert, item, signedIndex);
      expectThrow(list.replace, item, signedIndex);
      expectThrow(list.forEach, lambda, signedIndex)
    });
  }

  void testForEachFromStart(ListType list) {
    it("should loop through every item using the `forEach` method from start",
       [&]() {
         size_t lengthCount = 0;

         list.forEach([&](auto item, auto i) {
           expectEqual(static_cast<size_t>(item), i);
           lengthCount++;
         });

         expectEqual(lengthCount, list.getLength());
       });
  }

  void testForEachFromArbitraryIndex(ListType list) {
    it("should loop through every item using the `forEach` method from an "
       "arbitrary index",
       [&]() {
         const size_t startIndex = LIST_LENGTH / 2;
         size_t lengthCount = 0;

         list.forEach(
             [&](auto item, auto i) {
               expectEqual(static_cast<size_t>(item), i);
               lengthCount++;
             },
             startIndex);

         expectEqual(lengthCount, list.getLength() / 2);
       });
  }

  void testFind(ListType list) {
    it("should find an item", [&]() {
      const int itemToFind = LIST_LENGTH / 2;
      int item = -1;

      bool found =
          list.find([](auto item, auto) { return item == itemToFind; }, item);

      expectEqual(found, true);
      expectEqual(item, itemToFind);
    });
  }

  void testFindIndex(ListType list) {
    it("should find the index of an item", [&]() {
      const int itemToFind = LIST_LENGTH / 2;
      const size_t expectedIndex = itemToFind;
      size_t item = -1;

      bool found = list.findIndex(
          [](auto item, auto) { return item == itemToFind; }, item);

      expectEqual(found, true);
      expectEqual(item, expectedIndex);
    });
  }

  void testRemove(ListType list) {
    it("should remove an index twice", [&]() {
      const size_t indexToRemove = LIST_LENGTH - 10;

      {
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

      {
        const int indexValue = list.at(indexToRemove);
        const size_t expectedLength = LIST_LENGTH - 2;
        const int predecessorValue = list.at(indexToRemove - 1);
        const int successorValue = list.at(indexToRemove + 1);

        list.remove(indexToRemove);
        expectEqual(list.getLength(), expectedLength);
        expectDifer(list.at(indexToRemove), indexValue);
        expectEqual(list.at(indexToRemove), successorValue);
        expectEqual(list.at(indexToRemove - 1), predecessorValue);
      }
    });
  }

  void testRemoveZeroIndex(ListType list) {
    it("should remove first index", [&]() {
      const size_t indexToRemove = 0;
      const size_t expectedLength = LIST_LENGTH - 1;
      const int valueBeforeRemoving = list.at(indexToRemove);
      NodeStruct<int> *firstNodeBeforeRemoving;

      if constexpr (std::is_same_v<Derived, List<int>>) {
        firstNodeBeforeRemoving = list.firstNode;
      }

      list.remove(indexToRemove);
      expectEqual(list.getLength(), expectedLength);
      expectDifer(list.at(indexToRemove), valueBeforeRemoving);

      if constexpr (std::is_same_v<Derived, List<int>>) {
        expectDifer(list.firstNode, firstNodeBeforeRemoving)
      }
    });
  }

  void testRemoveLastIndex(ListType list) {
    it("should remove last index", [&]() {
      const size_t indexToRemove = LIST_LENGTH - 1;
      const size_t expectedLength = LIST_LENGTH - 1;
      const int predecessorValue = list.at(indexToRemove - 1);
      NodeStruct<int> *lastNodeBeforeRemoving;

      if constexpr (std::is_same_v<Derived, List<int>>) {
        lastNodeBeforeRemoving = list.lastNode;
      }

      list.remove(indexToRemove);
      expectEqual(list.getLength(), expectedLength);
      expectEqual(list.at(-1), predecessorValue);
      expectThrow(list.at, indexToRemove);

      if constexpr (std::is_same_v<Derived, List<int>>) {
        expectDifer(list.lastNode, lastNodeBeforeRemoving)
      }
    });
  }

  void testInsert(ListType list) {
    it("should insert an item", [&]() {
      const int valueToInsert = 50;
      const size_t indexToInsert = 10;
      const int indexValue = indexToInsert;
      const size_t expectedLength = LIST_LENGTH + 1;
      const int predecessorValue = indexToInsert - 1;
      const int successorValue = indexToInsert + 1;

      expectEqual(list.at(indexToInsert), indexValue);
      list.insert(valueToInsert, indexToInsert);

      expectEqual(list.at(indexToInsert), valueToInsert);
      expectEqual(list.at(indexToInsert + 1), indexValue);
      expectEqual(list.at(indexToInsert + 2), successorValue);
      expectEqual(list.at(indexToInsert - 1), predecessorValue);
      expectEqual(list.getLength(), expectedLength);
    });
  }

  void testReplace(ListType list) {
    it("should replace an item", [&]() {
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
    });
  }

  void testInsertWithZeroLength() {
    it("should insert an item when length is zero", []() {
      Derived derived;

      expectEqual(derived.getLength(), 0);

      derived.insert(0);

      expectEqual(derived.at(0), 0);
    });
  }

  void testInsertLastIndex(ListType list) {
    it("should insert an item in index LIST_LENGTH - 1", [&]() {
      const size_t indexToInsert = LIST_LENGTH - 1;
      const int itemToInsert = 10;
      NodeStruct<int> *lastNodeBeforeRemoving;

      if constexpr (std::is_same_v<Derived, List<int>>) {
        lastNodeBeforeRemoving = list.lastNode;
      }

      list.insert(itemToInsert, indexToInsert);
      expectEqual(list.getLength(), LIST_LENGTH + 1);
      expectEqual(itemToInsert, list.at(indexToInsert));

      if constexpr (std::is_same_v<Derived, List<int>>) {
        expectDifer(list.lastNode, lastNodeBeforeRemoving)
      }
    });
  }

  std::vector<std::function<void(ListType)>> testsList = {
      std::bind(&TestBaseListDerivedClass::testAccessOperator, this,
                std::placeholders::_1),
      std::bind(&TestBaseListDerivedClass::testForEachFromArbitraryIndex, this,
                std::placeholders::_1),
      std::bind(&TestBaseListDerivedClass::testForEachFromStart, this,
                std::placeholders::_1),
      std::bind(&TestBaseListDerivedClass::testFindIndex, this,
                std::placeholders::_1),
      std::bind(&TestBaseListDerivedClass::testFind, this,
                std::placeholders::_1),
      std::bind(
          &TestBaseListDerivedClass::testAtMethodWithPositiveAndNegativeIndices,
          this, std::placeholders::_1),
      std::bind(&TestBaseListDerivedClass::testOutOfRangeThrow, this,
                std::placeholders::_1),
      std::bind(&TestBaseListDerivedClass::testInsert, this,
                std::placeholders::_1),
      std::bind(&TestBaseListDerivedClass::testRemove, this,
                std::placeholders::_1),
      std::bind(&TestBaseListDerivedClass::testReplace, this,
                std::placeholders::_1),
      std::bind(&TestBaseListDerivedClass::testInsertLastIndex, this,
                std::placeholders::_1),
      std::bind(&TestBaseListDerivedClass::testRemoveZeroIndex, this,
                std::placeholders::_1),
      std::bind(&TestBaseListDerivedClass::testRemoveLastIndex, this,
                std::placeholders::_1),
  };

public:
  static void mockBaseListDerivedClass(BaseList<int, Derived> &derived) {
    for (size_t i = 0; i < LIST_LENGTH; i++) {
      derived.push(i);
    }
  }

  void executeTests() {
    for (auto test : testsList) {
      auto list = beforeEachTest();
      test(list);
    }

    testInsertWithZeroLength();
  };

  static Derived initBaseListDerivedClass() {
    Derived derived;

    if constexpr (std::is_same_v<Derived, Vector<int>>) {
      derived.reserve(LIST_LENGTH);
    }

    mockBaseListDerivedClass(derived);
    return derived;
  }
};

int main() {
  describe("Vector", []() {
    TestBaseListDerivedClass<Vector<int>> vectorTests;

    it("should reserve the the correct capacity", [&]() {
      Vector<int> vec;
      vec.reserve(LIST_LENGTH);
      expectEqual(vec.getCapacity(), LIST_LENGTH);
    });

    it("should push items", [&]() {
      auto vec =
          TestBaseListDerivedClass<Vector<int>>::initBaseListDerivedClass();
      expectEqual(vec.getLength(), LIST_LENGTH);
    });

    it("should shrink to fit", [&]() {
      Vector<int> vec;
      TestBaseListDerivedClass<Vector<int>>::mockBaseListDerivedClass(vec);
      const size_t capacityBeforeShrinking = vec.getCapacity();

      vec.shrinkToFit();

      expectGreaterThan(capacityBeforeShrinking, vec.getCapacity());
    });

    vectorTests.executeTests();
  });

  describe("List", []() {
    TestBaseListDerivedClass<List<int>> listTests;

    listTests.executeTests();
  });

  return 0;
}
