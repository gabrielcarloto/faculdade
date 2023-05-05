#include "BaseList.h"
#include <functional>
#include <type_traits>

template <typename T, class BaseListDerived = BaseList<T>> class SortedList {
  using DerivedRef = BaseListDerived &;

public:
  using SortFunction = std::function<void(DerivedRef)>;
  using CompareFunction = std::function<short int(T &)>;

  SortedList(const SortFunction &fn) : sortFn(fn) {
    assertListDerivedFromBaseList();
  };

  void setSortFunction(const SortFunction &fn) { sortFn = fn; }

  void sort() { sortFn(list); };

  void add(const T &);
  void add(const T &&);
  auto search(const CompareFunction &) -> T &;
  auto searchIndex(const CompareFunction &) -> size_t;

  auto operator->() -> BaseListDerived * { return &list; }

  static void bubbleSort(DerivedRef list) {
    for (size_t lastSortedIndex = list.getLength(); lastSortedIndex > 0;
         lastSortedIndex--) {
      for (size_t i = 0; i < lastSortedIndex - 1; i++) {
        if (list[i] > list[i + 1])
          swap(list[i], list[i + 1]);
      }
    }
  }

  static void selectionSort(DerivedRef list) {
    for (size_t i = 0; i < list.getLength() - 1; i++) {
      size_t minValueIndex = i;

      for (size_t j = i + 1; j < list.getLength(); j++)
        if (list[j] < list[minValueIndex])
          minValueIndex = j;

      swap(list[i], list[minValueIndex]);
    }
  }

  static void insertionSort(DerivedRef list) {
    for (size_t i = 1; i < list.getLength(); i++) {
      T temp = list[i];
      for (size_t j = i; j > 0 && temp < list[j - 1]; j--) {
        swap(list[j], list[j - 1]);
      }
    }
  }

  static void shellSort(DerivedRef list) {
    for (size_t interval = list.length() / 2; interval > 0; interval /= 2) {
      for (size_t i = interval; i < list.length(); i++) {
        T temp = list[i];

        // clang-format off
        for (size_t j = i; j >= interval && temp < list[j - interval]; j -= interval)
          // clang-format on
          swap(list[j], list[j - interval]);
      }
    }
  }

  static void quickSort(DerivedRef);
  static void mergeSort(DerivedRef);

  static void swap(T &a, T &b) {
    T temp = std::move(a);
    a = std::move(b);
    b = std::move(temp);
  }

private:
  BaseListDerived list;
  SortFunction sortFn;

  void assertListDerivedFromBaseList() {
    static_assert(std::is_base_of<BaseList<T>, BaseListDerived>::value,
                  "Template BaseListDerived is not derived from BaseList");
  }
};
