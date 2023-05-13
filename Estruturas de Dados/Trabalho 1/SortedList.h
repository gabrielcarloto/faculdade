#include "BaseList.h"
#include "List.h"
#include "Vector.h"
#include <functional>
#include <future>
#include <type_traits>

template <typename T, class BaseListDerived> class SortedList {
  using DerivedRef = BaseListDerived &;

public:
  using SortFunction = std::function<void(DerivedRef)>;
  using SearchCompareFunction = std::function<short int(T &, T &)>;

  SortedList(const SortFunction &fn) : sortFn(fn) {
    assertListDerivedFromBaseList();
  };

  auto getList() const -> BaseListDerived & { return list; }
  void setSortFunction(const SortFunction &fn) { sortFn = fn; }

  void sort() { sortFn(list); };

  void add(const T &);
  void add(const T &&);
  auto search(const SearchCompareFunction &) -> T &;
  auto searchIndex(const SearchCompareFunction &) -> size_t;

  auto operator->() -> BaseListDerived * { return &list; }

  typename BaseListDerived::TIterator begin() { return list.begin(); }
  typename BaseListDerived::TIterator end() { return list.end(); }

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
    for (size_t interval = list.getLength() / 2; interval > 0; interval /= 2) {
      for (size_t i = interval; i < list.getLength(); i++) {
        T temp = list[i];

        // clang-format off
        for (size_t j = i; j >= interval && temp < list[j - interval]; j -= interval)
          // clang-format on
          swap(list[j], list[j - interval]);
      }
    }
  }

  inline static void quickSort(DerivedRef list) { QuickSort::sort(list); }

  inline static void mergeSort(DerivedRef list) { MergeSort::sort(list); }

  static void swap(T &a, T &b) {
    T temp = std::move(a);
    a = std::move(b);
    b = std::move(temp);
  }

private:
  BaseListDerived list;
  SortFunction sortFn;

  void assertListDerivedFromBaseList() {
    static_assert(
        std::is_base_of<
            BaseList<T, BaseListDerived, typename BaseListDerived::TIterator>,
            BaseListDerived>::value,
        "Template BaseListDerived is not derived from BaseList");
  }

  static const unsigned int maxThreads;

  class QuickSort {
    static void quickSort(DerivedRef list, size_t start, size_t end) {
      if (start >= end)
        return;

      auto pivotIndex = partition(list, start, end);

      quickSort(list, start, pivotIndex);
      quickSort(list, pivotIndex + 1, end);
    }

    static size_t partition(DerivedRef list, size_t start, size_t end) {
      size_t pivotIndex = medianThree(list, start, end), partitionIndex = start;

      swap(list[pivotIndex], list[end]);
      const T &pivotValue = list[end];

      for (size_t i = start; i < end; i++) {
        if (list[i] < pivotValue) {
          swap(list[i], list[partitionIndex]);
          partitionIndex++;
        }
      }

      swap(list[partitionIndex], list[end]);
      return partitionIndex;
    }

    // clang-format off
  inline static size_t medianThree(DerivedRef list, size_t start, size_t end) {
      // clang-format on
      size_t mid = (start + end) / 2;
      const T &startValue = list[start], &midValue = list[mid],
              &endValue = list[end];

      if ((startValue > midValue) ^ (startValue > endValue))
        return start;
      if ((midValue < startValue) ^ (midValue < endValue))
        return mid;

      return end;
    }

  public:
    inline static void sort(DerivedRef list) {
      quickSort(list, 0, list.getLength() - 1);
    }
  };

  class MergeSort {
    static void merge(DerivedRef results, DerivedRef temp, intmax_t start,
                      intmax_t mid, intmax_t end) {
      intmax_t endLeft = mid - 1, tempPos = start, size = end - start + 1;

      while (start <= endLeft && mid <= end) {
        if (results[start] <= results[mid]) {
          temp[tempPos] = results[start];
          tempPos++;
          start++;
        } else {
          temp[tempPos] = results[mid];
          tempPos++;
          mid++;
        }
      }

      while (start <= endLeft) {
        temp[tempPos] = results[start];
        tempPos++;
        start++;
      }

      while (mid <= end) {
        temp[tempPos] = results[mid];
        tempPos++;
        mid++;
      }

      for (intmax_t i = 0; i < size; i++) {
        results[end] = temp[end];
        end--;
      }
    }

    static void mergeSortNoThread(DerivedRef list, DerivedRef temp,
                                  size_t start, size_t end) {
      if (end <= start)
        return;

      size_t mid = (start + end) / 2;
      mergeSortNoThread(list, temp, start, mid);
      mergeSortNoThread(list, temp, mid + 1, end);
      merge(list, temp, (intmax_t)start, (intmax_t)mid + 1, (intmax_t)end);
    }

    static void mergeSort(DerivedRef list, DerivedRef temp, size_t start,
                          size_t end, unsigned int currentThreads = 1,
                          bool shouldThread = true) {
      if (end <= start)
        return;

      size_t mid = (start + end) / 2;

      if (shouldThread && currentThreads < maxThreads) {
        auto thread =
            std::async(std::launch::async, mergeSort, std::ref(list),
                       std::ref(temp), start, mid, currentThreads * 2, true);

        mergeSort(list, temp, mid + 1, end, currentThreads * 2, true);
        thread.wait();
      } else {
        mergeSortNoThread(list, temp, start, mid);
        mergeSortNoThread(list, temp, mid + 1, end);
      }

      merge(list, temp, (intmax_t)start, (intmax_t)mid + 1, (intmax_t)end);
    }

  public:
    static void sort(DerivedRef list) {
      BaseListDerived temp;

      if constexpr (std::is_same_v<BaseListDerived, Vector<T>>)
        temp.reserve(list.getLength());
      else if constexpr (std::is_same_v<BaseListDerived, List<T>>)
        temp.allocate(list.getLength());

      mergeSort(list, temp, 0, list.getLength() - 1, 1, list.getLength() > 200);
    }
  };
};

// clang-format off
template <typename T, class B>
const unsigned int SortedList<T, B>::maxThreads(std::thread::hardware_concurrency());
