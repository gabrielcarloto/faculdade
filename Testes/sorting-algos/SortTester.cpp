#include <algorithm>
#include <array>
#include <chrono>
#include <functional>
#include <iostream>
#include <random>
#include <vector>

#define VEC_LEN 12000000 // Length of the vector for large test cases

// nice AI generated code 👍
class SortTester {
public:
  SortTester(const std::string &algorithm_name,
             std::function<void(std::vector<int> &)> sort_algorithm)
      : algorithm_name(algorithm_name), sort_algorithm(sort_algorithm) {}

  void runTests() {
    std::cout << "Running tests for " << algorithm_name << ":" << std::endl;
    runTest(test_case_1, expected_output_1);
    runTest(test_case_2, expected_output_2);
    runTest(test_case_3, expected_output_3);
    runTest(test_case_4, expected_output_4);
    runTest(test_case_5, expected_output_5);
    runLargeTest();
  }

  bool isSorted(std::vector<int> &vec) {
    for (size_t i = 0; i < vec.size() - 1; i++) {
      if (vec[i] > vec[i + 1]) {
        return false;
      }
    }
    return true;
  }

private:
  unsigned int currentTestCase = 0;
  const std::string algorithm_name;
  std::function<void(std::vector<int> &)> sort_algorithm;

  // Test cases for small arrays
  std::vector<int> test_case_1{5, 3, 8, 6, 2, 7, 1, 4};
  std::vector<int> expected_output_1{1, 2, 3, 4, 5, 6, 7, 8};

  std::vector<int> test_case_2{1, 2, 3, 4, 5, 6, 7, 8};
  std::vector<int> expected_output_2{1, 2, 3, 4, 5, 6, 7, 8};

  std::vector<int> test_case_3{8, 7, 6, 5, 4, 3, 2, 1};
  std::vector<int> expected_output_3{1, 2, 3, 4, 5, 6, 7, 8};

  std::vector<int> test_case_4{5, 3, 8, 6, 2, 7, 1, 4, 5};
  std::vector<int> expected_output_4{1, 2, 3, 4, 5, 5, 6, 7, 8};

  std::vector<int> test_case_5{5, -3, 8, -6, 2, 7, -1, 4};
  std::vector<int> expected_output_5{-6, -3, -1, 2, 4, 5, 7, 8};

  void runTest(const std::vector<int> &test_case,
               const std::vector<int> &expected_output) {
    currentTestCase++;
    std::vector<int> vec = test_case;
    auto start_time = std::chrono::high_resolution_clock::now();
    sort_algorithm(vec);
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
                        end_time - start_time)
                        .count();
    bool success = vec == expected_output;
    std::cout << "\tTest case " << currentTestCase << ": "
              << (success ? "succeeded" : "failed") << " for " << algorithm_name
              << ", took " << duration << "ms." << std::endl;
  }

  void runLargeTest() {
    std::vector<int> vec(VEC_LEN);
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(-10000, 10000);
    std::generate(vec.begin(), vec.end(),
                  [&dist, &rng]() { return dist(rng); });

    auto start_time = std::chrono::high_resolution_clock::now();
    sort_algorithm(vec);
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
                        end_time - start_time)
                        .count();
    bool success = isSorted(vec);

    std::cout << "\tLarge test case " << (success ? "succeeded" : "failed")
              << " for " << algorithm_name << ", took " << duration << "ms."
              << std::endl;
  }
};
