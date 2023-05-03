#include "BaseList.h"
#include <functional>
#include <type_traits>

template <typename T, class BaseListDerived = BaseList<T>> class SortedList {
  using DerivedRef = BaseListDerived &;

public:
  using SortFunction = std::function<void(DerivedRef)>;
  using CompareFunction = std::function<int(T &)>;

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

  static void bubbleSort(DerivedRef list);

  static void selectionSort(DerivedRef);
  static void insertionSort(DerivedRef);
  static void shellSort(DerivedRef);
  static void quickSort(DerivedRef);
  static void mergeSort(DerivedRef);

  static void swap(T &a, T &b) {
    T temp = std::move(a);
    a = b;
    b = temp;
  }

private:
  BaseListDerived list;
  SortFunction sortFn;

  void assertListDerivedFromBaseList() {
    static_assert(std::is_base_of<BaseList<T>, BaseListDerived>::value,
                  "Template BaseListDerived is not derived from BaseList");
  }
};
