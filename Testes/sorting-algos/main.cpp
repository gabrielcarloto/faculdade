#include "SortTester.cpp"
#include <future>

const unsigned int maxThreads = std::thread::hardware_concurrency();

void insertionSort(std::vector<int> &vec) {
  for (size_t i = 1; i < vec.size(); i++) {
    int temp = vec[i];
    for (size_t j = i; j > 0 && temp < vec[j - 1]; j--) {
      std::swap(vec[j], vec[j - 1]);
    }
  }
}

void bubbleSort(std::vector<int> &vec) {
  for (size_t i = vec.size(); i > 0; i--)
    for (size_t j = 1; j < i; j++)
      if (vec[j] < vec[j - 1])
        std::swap(vec[j], vec[j - 1]);
}

void selectionSort(std::vector<int> &vec) {
  for (size_t i = 0; i < vec.size() - 1; i++) {
    size_t minValueIndex = i;

    for (size_t j = i + 1; j < vec.size(); j++)
      if (vec[j] < vec[minValueIndex])
        minValueIndex = j;

    std::swap(vec[i], vec[minValueIndex]);
  }
}

class BinaryInsertionSort {
  static size_t searchIndexForItem(std::vector<int> &vec, int item,
                                   size_t start = 0, size_t end = 0) {
    size_t mid = 1;

    while (start < end && mid > 0) {
      mid = (start + end) / 2;

      if (vec[mid] == item)
        return mid;

      if (item < vec[mid])
        end = mid - 1;
      else
        start = mid + 1;
    }

    return start + (item > vec[start]);
  }

public:
  static void sort(std::vector<int> &vec) {
    for (size_t i = 1; i < vec.size(); i++) {
      int temp = vec[i];
      if (temp < vec[i - 1]) {
        auto index = searchIndexForItem(vec, temp, 0, i - 1);

        for (size_t j = i; j > index; j--)
          vec[j] = vec[j - 1];

        vec[index] = temp;
      }
    }
  }
};

class QuickSort {
  static void quickSortNoThread(std::vector<int> &list, size_t start,
                                size_t end) {
    if (start >= end)
      return;

    auto pivotIndex = partition(list, start, end);

    quickSortNoThread(list, start, pivotIndex);
    quickSortNoThread(list, pivotIndex + 1, end);
  }

  static void quickSort(std::vector<int> &list, size_t start, size_t end,
                        unsigned int currentThreads = 1,
                        bool shouldThread = true) {
    if (start >= end)
      return;

    auto pivotIndex = partition(list, start, end);

    if (shouldThread && currentThreads < maxThreads) {
      auto thread = std::async(std::launch::async, quickSort, std::ref(list),
                               start, pivotIndex, currentThreads * 2, true);

      quickSort(list, pivotIndex + 1, end, currentThreads * 2, true);
      thread.wait();
    } else {
      quickSortNoThread(list, start, pivotIndex);
      quickSortNoThread(list, pivotIndex + 1, end);
    }
  }

  static size_t partition(std::vector<int> &list, size_t start, size_t end) {
    size_t pivotIndex = medianOfThree(list, start, end), partitionIndex = start;

    std::swap(list[pivotIndex], list[end]);
    const int &pivotValue = list[end];

    for (size_t i = start; i < end; i++) {
      if (list[i] < pivotValue) {
        std::swap(list[i], list[partitionIndex]);
        partitionIndex++;
      }
    }

    std::swap(list[partitionIndex], list[end]);
    return partitionIndex;
  }

  // clang-format off
  inline static size_t medianOfThree(std::vector<int>& list, size_t start, size_t end) {
    // clang-format on
    size_t mid = (start + end) / 2;

    if ((list[start] > list[mid]) ^ (list[start] > list[end]))
      return start;
    if ((list[mid] < list[start]) ^ (list[mid] < list[end]))
      return mid;

    return end;
  }

public:
  static void sort(std::vector<int> &vec) {
    quickSort(vec, 0, vec.size() - 1, 1, vec.size() > 200);
  }
};

class MergeSort {
  static void merge(std::vector<int> &results, std::vector<int> &temp,
                    intmax_t start, intmax_t mid, intmax_t end) {
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

  static void mergeSortNoThread(std::vector<int> &vec, std::vector<int> &temp,
                                size_t start, size_t end) {
    if (end <= start)
      return;

    size_t mid = (start + end) / 2;
    mergeSortNoThread(vec, temp, start, mid);
    mergeSortNoThread(vec, temp, mid + 1, end);
    merge(vec, temp, (intmax_t)start, (intmax_t)mid + 1, (intmax_t)end);
  }

  static void mergeSort(std::vector<int> &vec, std::vector<int> &temp,
                        size_t start, size_t end,
                        unsigned int currentThreads = 1,
                        bool shouldThread = true) {
    if (end <= start)
      return;

    size_t mid = (start + end) / 2;

    if (shouldThread && currentThreads < maxThreads) {
      auto thread =
          std::async(std::launch::async, mergeSort, std::ref(vec),
                     std::ref(temp), start, mid, currentThreads * 2, true);

      mergeSort(vec, temp, mid + 1, end, currentThreads * 2, true);
      thread.wait();
    } else {
      mergeSortNoThread(vec, temp, start, mid);
      mergeSortNoThread(vec, temp, mid + 1, end);
    }

    merge(vec, temp, (intmax_t)start, (intmax_t)mid + 1, (intmax_t)end);
  }

public:
  static void sort(std::vector<int> &vec) {
    std::vector<int> temp;
    temp.reserve(vec.size());

    mergeSort(vec, temp, 0, vec.size() - 1, 1, vec.size() > 200);
  }
};

int main() {
  SortTester insertionSortTester("InsertionSort", insertionSort),
      bubbleSortTester("BubbleSort", bubbleSort),
      selectionSortTester("SelectionSort", selectionSort),
      binaryInsertionSortTester("BinaryInsertionSort",
                                BinaryInsertionSort::sort),
      quickSortTester("QuickSort", QuickSort::sort),
      mergeSortTester("MergeSort", MergeSort::sort);

  // insertionSortTester.runTests();
  // binaryInsertionSortTester.runTests();
  // bubbleSortTester.runTests();
  // selectionSortTester.runTests();
  quickSortTester.runTests();
  mergeSortTester.runTests();

  return 0;
}
