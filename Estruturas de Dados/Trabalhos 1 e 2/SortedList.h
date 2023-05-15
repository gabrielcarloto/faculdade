#include "BaseList.h"
#include "List.h"
#include "Profiler.h"
#include "Vector.h"
#include <functional>
#include <future>
#include <type_traits>

template <typename T, class BaseListDerived> class SortedList {
  using DerivedRef = BaseListDerived &;

public:
  struct ValueCompareFunctions {
    std::function<bool(const T &, const T &)> less =
        [](const T &a, const T &b) -> bool { return a < b; };

    std::function<bool(const T &, const T &)> lessEqual =
        [](const T &a, const T &b) -> bool { return a <= b; };

    std::function<bool(const T &, const T &)> greater =
        [](const T &a, const T &b) -> bool { return a > b; };

    std::function<bool(const T &, const T &)> greaterEqual =
        [](const T &a, const T &b) -> bool { return a >= b; };

    std::function<bool(const T &, const T &)> equal =
        [](const T &a, const T &b) -> bool { return a == b; };
  };

  using SortFunction =
      std::function<void(DerivedRef, Profiler &, ValueCompareFunctions &)>;

  SortedList(const SortFunction &fn) : sortFn(fn) {
    assertListDerivedFromBaseList();
  };

  SortedList(const SortFunction &fn, ValueCompareFunctions &valFn)
      : sortFn(fn), comp(std::move(valFn)) {
    assertListDerivedFromBaseList();
  };

  auto getList() const -> BaseListDerived & { return list; }
  void setSortFunction(const SortFunction &fn) { sortFn = std::move(fn); }
  auto getProfiler() const -> const Profiler & { return profiler; }
  [[nodiscard]] auto getLength() const -> size_t { return list.getLength(); }

  void sort() {
    profiler.start();
    sortFn(list, profiler, comp);
    profiler.end();
  };

  T &at(size_t index) { return list.at(index); }

  size_t add(const T &item) {
    profiler.start();
    size_t index = searchIndexForItem(item);
    list.insert(item, index);
    profiler.end();
    return index;
  }

  size_t add(const T &&item) {
    profiler.start();
    size_t index = searchIndexForItem(item);
    list.insert(item, index);
    profiler.end();
    return index;
  }

  auto search(const T &item, bool *found) -> T & {
    size_t index = searchIndex(item, found);
    return list[index];
  }

  auto searchIndex(const T &item, bool *found) -> size_t {
    profiler.start();
    size_t start = 0, end = list.getLength() - 1, mid = (start + end) / 2;

    while (start < end && mid > 0) {
      mid = (start + end) / 2;

      profiler.addComparison();
      if (comp.equal(list[mid], item)) {
        *found = true;
        profiler.end();
        return mid;
      }

      profiler.addComparison();
      if (comp.less(item, list[mid]))
        end = mid - 1;
      else
        start = mid + 1;
    }

    profiler.addComparison();
    if (comp.equal(list[start], item)) {
      *found = true;
      profiler.end();
      return start;
    }

    *found = false;
    profiler.end();
    return 0;
  }

  auto operator->() -> BaseListDerived * { return &list; }
  auto operator[](size_t index) -> T & { return list[index]; }

  typename BaseListDerived::TIterator begin() { return list.begin(); }
  typename BaseListDerived::TIterator end() { return list.end(); }

  static void bubbleSort(DerivedRef list, Profiler &prof,
                         ValueCompareFunctions &comp) {
    for (size_t lastSortedIndex = list.getLength(); lastSortedIndex > 0;
         lastSortedIndex--) {
      prof.addComparison();
      for (size_t i = 0; i < lastSortedIndex - 1; i++) {
        prof.addComparison();
        if (comp.greater(list[i], list[i + 1]))
          swap(list[i], list[i + 1], prof);
      }
    }
  }

  static void selectionSort(DerivedRef list, Profiler &prof,
                            ValueCompareFunctions &comp) {
    for (size_t i = 0; i < list.getLength() - 1; i++) {
      size_t minValueIndex = i;
      prof.addComparison();

      for (size_t j = i + 1; j < list.getLength(); j++) {
        prof.addComparison();
        if (comp.less(list[j], list[minValueIndex]))
          minValueIndex = j;
      }

      swap(list[i], list[minValueIndex], prof);
    }
  }

  static void insertionSort(DerivedRef list, Profiler &prof,
                            ValueCompareFunctions &comp) {
    for (size_t i = 1; i < list.getLength(); i++) {
      prof.addComparison();
      T temp = list[i];

      for (size_t j = i; j > 0 && comp.less(temp, list[j - 1]); j--) {
        swap(list[j], list[j - 1], prof);
        prof.addComparison();
      }
    }
  }

  static void shellSort(DerivedRef list, Profiler &prof,
                        ValueCompareFunctions &comp) {
    for (size_t interval = list.getLength() / 2; interval > 0; interval /= 2) {
      prof.addComparison();
      for (size_t i = interval; i < list.getLength(); i++) {
        prof.addComparison();
        T temp = list[i];

        // clang-format off
        for (size_t j = i; j >= interval && comp.less(temp, list[j - interval]); j -= interval) {
          // clang-format on
          prof.addComparison();
          swap(list[j], list[j - interval], prof);
        }
      }
    }
  }

  static inline void quickSort(DerivedRef list, Profiler &prof,
                               ValueCompareFunctions &comp) {
    QuickSort::sort(list, prof, comp);
  }

  static inline void mergeSort(DerivedRef list, Profiler &prof,
                               ValueCompareFunctions &comp) {
    MergeSort::sort(list, prof, comp);
  }

  static void swap(T &a, T &b, Profiler &prof) {
    prof.addMove();
    T temp = std::move(a);
    a = std::move(b);
    b = std::move(temp);
  }

private:
  BaseListDerived list;
  SortFunction sortFn;
  mutable Profiler profiler;
  ValueCompareFunctions comp;

  void assertListDerivedFromBaseList() {
    static_assert(
        std::is_base_of<
            BaseList<T, BaseListDerived, typename BaseListDerived::TIterator>,
            BaseListDerived>::value,
        "Template BaseListDerived is not derived from BaseList");
  }

  static const unsigned int maxThreads;

  size_t searchIndexForItem(const T &item) {
    size_t start = 0, end = list.getLength() - 1, mid = (start + end) / 2;

    while (start < end && mid > 0) {
      profiler.addComparison();
      mid = (start + end) / 2;

      profiler.addComparison();
      if (comp.equal(list[mid], item))
        return mid;

      profiler.addComparison();
      if (comp.less(item, list[mid]))
        end = mid - 1;
      else
        start = mid + 1;
    }

    profiler.addComparison();
    return start + comp.greater(item, list[start]);
  }

  class QuickSort {
    static void quickSortNoThread(DerivedRef list, size_t start, size_t end,
                                  Profiler &prof, ValueCompareFunctions &comp) {
      prof.addComparison();
      if (start >= end)
        return;

      auto pivotIndex = partition(list, start, end, prof, comp);

      quickSortNoThread(list, start, pivotIndex, prof, comp);
      quickSortNoThread(list, pivotIndex + 1, end, prof, comp);
    }

    static void quickSort(DerivedRef list, size_t start, size_t end,
                          Profiler &prof, ValueCompareFunctions &comp,
                          unsigned int currentThreads = 1,
                          bool shouldThread = false) {
      prof.addComparison();
      if (start >= end)
        return;

      auto pivotIndex = partition(list, start, end, prof, comp);

      prof.addComparison();
      if (shouldThread && currentThreads < maxThreads) {
        auto thread = std::async(std::launch::async, quickSort, std::ref(list),
                                 start, pivotIndex, std::ref(prof),
                                 std::ref(comp), currentThreads * 2, true);

        quickSort(list, pivotIndex + 1, end, prof, comp, currentThreads * 2,
                  true);
        thread.wait();
      } else {
        quickSortNoThread(list, start, pivotIndex, prof, comp);
        quickSortNoThread(list, pivotIndex + 1, end, prof, comp);
      }
    }

    // clang-format off
    static size_t partition(DerivedRef list, size_t start, size_t end, Profiler &prof, ValueCompareFunctions &comp) {
      // clang-format on
      size_t pivotIndex = medianThree(list, start, end, prof, comp),
             partitionIndex = start;

      swap(list[pivotIndex], list[end], prof);
      const T &pivotValue = list[end];

      for (size_t i = start; i < end; i++) {
        prof.addComparison(2);
        if (comp.less(list[i], pivotValue)) {
          swap(list[i], list[partitionIndex], prof);
          partitionIndex++;
        }
      }

      swap(list[partitionIndex], list[end], prof);
      return partitionIndex;
    }

    inline static size_t medianThree(DerivedRef list, size_t start, size_t end,
                                     Profiler &prof,
                                     ValueCompareFunctions &comp) {
      size_t mid = (start + end) / 2;
      const T &startValue = list[start], &midValue = list[mid],
              &endValue = list[end];

      prof.addComparison();
      if (comp.greater(startValue, midValue) ^
          comp.greater(startValue, endValue))
        return start;

      prof.addComparison();
      if (comp.less(midValue, startValue) ^ comp.less(midValue, endValue))
        return mid;

      return end;
    }

  public:
    inline static void sort(DerivedRef list, Profiler &prof,
                            ValueCompareFunctions &comp) {
      bool shouldThread = list.getLength() > 200;

      if constexpr (std::is_same_v<BaseListDerived, List<T>>)
        shouldThread = false;

      prof.addComparison();
      quickSort(list, 0, list.getLength() - 1, prof, comp, 1, shouldThread);
    }
  };

  class MergeSort {
    static void merge(DerivedRef results, DerivedRef temp, intmax_t start,
                      intmax_t mid, intmax_t end, Profiler &prof,
                      ValueCompareFunctions &comp) {
      intmax_t endLeft = mid - 1, tempPos = start, size = end - start + 1;

      while (start <= endLeft && mid <= end) {
        prof.addComparison(2);
        prof.addMove();

        T &tempValue = temp[tempPos], &resStart = results[start],
          &resMid = results[mid];

        if (comp.lessEqual(resStart, resMid)) {
          tempValue = resStart; // cópia conta como movimentação?
          tempPos++;
          start++;
        } else {
          tempValue = resMid;
          tempPos++;
          mid++;
        }
      }

      while (start <= endLeft) {
        prof.addComparison();
        prof.addMove();
        temp[tempPos] = results[start];
        tempPos++;
        start++;
      }

      while (mid <= end) {
        prof.addComparison();
        prof.addMove();
        temp[tempPos] = results[mid];
        tempPos++;
        mid++;
      }

      for (intmax_t i = 0; i < size; i++) {
        prof.addComparison();
        prof.addMove();
        results[end] = temp[end];
        end--;
      }
    }

    static void mergeSortNoThread(DerivedRef list, DerivedRef temp,
                                  size_t start, size_t end, Profiler &prof,
                                  ValueCompareFunctions &comp) {
      prof.addComparison();
      if (end <= start)
        return;

      size_t mid = (start + end) / 2;
      mergeSortNoThread(list, temp, start, mid, prof, comp);
      mergeSortNoThread(list, temp, mid + 1, end, prof, comp);
      merge(list, temp, (intmax_t)start, (intmax_t)mid + 1, (intmax_t)end, prof,
            comp);
    }

    static void mergeSort(DerivedRef list, DerivedRef temp, size_t start,
                          size_t end, Profiler &prof,
                          ValueCompareFunctions &comp,
                          unsigned int currentThreads = 1,
                          bool shouldThread = true) {
      prof.addComparison();
      if (end <= start)
        return;

      size_t mid = (start + end) / 2;

      prof.addComparison();
      if (shouldThread && currentThreads < maxThreads) {
        auto thread = std::async(std::launch::async, mergeSort, std::ref(list),
                                 std::ref(temp), start, mid, std::ref(prof),
                                 std::ref(comp), currentThreads * 2, true);

        mergeSort(list, temp, mid + 1, end, prof, comp, currentThreads * 2,
                  true);
        thread.wait();
      } else {
        mergeSortNoThread(list, temp, start, mid, prof, comp);
        mergeSortNoThread(list, temp, mid + 1, end, prof, comp);
      }

      merge(list, temp, (intmax_t)start, (intmax_t)mid + 1, (intmax_t)end, prof,
            comp);
    }

  public:
    static void sort(DerivedRef list, Profiler &prof,
                     ValueCompareFunctions &comp) {
      BaseListDerived temp;
      bool shouldThread = list.getLength() > 200;

      if constexpr (std::is_same_v<BaseListDerived, Vector<T>>) {
        temp.reserve(list.getLength());
      } else if constexpr (std::is_same_v<BaseListDerived, List<T>>) {
        temp.allocate(list.getLength());
        // BUG: as threads atualmente estão levando a um data race. Por
        // enquanto, apenas tirei o multithreading do List pois em algum momento
        // o gotoIndex retornava um nullptr, crashando o programa
        shouldThread = false;
      }

      mergeSort(list, temp, 0, list.getLength() - 1, prof, comp, 1,
                shouldThread);
    }
  };
};

// clang-format off
template <typename T, class B>
const unsigned int SortedList<T, B>::maxThreads(std::thread::hardware_concurrency());
