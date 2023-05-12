#include "SortTester.cpp"

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
  static void quickSort(std::vector<int> &list, size_t start, size_t end) {
    if (start >= end)
      return;

    auto pivotIndex = partition(list, start, end);

    quickSort(list, start, pivotIndex);
    quickSort(list, pivotIndex + 1, end);
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
  static void sort(std::vector<int> &vec) { quickSort(vec, 0, vec.size() - 1); }
};

int main() {
  SortTester insertionSortTester("InsertionSort", insertionSort),
      bubbleSortTester("BubbleSort", bubbleSort),
      selectionSortTester("SelectionSort", selectionSort),
      binaryInsertionSortTester("BinaryInsertionSort",
                                BinaryInsertionSort::sort),
      quickSortTester("QuickSort", QuickSort::sort);

  insertionSortTester.runTests();
  binaryInsertionSortTester.runTests();
  // bubbleSortTester.runTests();
  // selectionSortTester.runTests();
  quickSortTester.runTests();

  return 0;
}
