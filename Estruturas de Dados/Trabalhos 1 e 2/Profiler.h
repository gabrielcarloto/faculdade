#pragma once
#include <chrono>
#include <iostream>

class Profiler {
  using clock = std::chrono::high_resolution_clock;
  using timePoint = std::chrono::high_resolution_clock::time_point;

  timePoint startTime, endTime;
  unsigned int comparisons = 0, moves = 0;

  std::string name;

  bool checkTimepoints();

public:
  Profiler();

  void start();

  void end();

  void addComparison(unsigned int n = 1);

  void addMove(unsigned int n = 1);

  std::chrono::milliseconds getDuration() const;
  unsigned int getComparisons() const;
  unsigned int getMoves() const;

  std::string getName() const;
  void setName(std::string);
  void printInfo(bool = true);
};
