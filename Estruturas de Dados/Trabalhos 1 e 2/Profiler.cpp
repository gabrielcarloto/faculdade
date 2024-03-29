#include "Profiler.h"
#include "utils.h"
#include <chrono>
#include <iostream>

bool Profiler::checkTimepoints() { return endTime <= startTime; }

Profiler::Profiler() {
  start();
  endTime = startTime;
}

void Profiler::start() {
  comparisons = 0;
  moves = 0;

  startTime = clock::now();
}

void Profiler::end() { endTime = clock::now(); }

void Profiler::addComparison(unsigned int n) {
  if (checkTimepoints())
    comparisons += n;
}

void Profiler::addMove(unsigned int n) {
  if (checkTimepoints())
    moves += n;
}

std::chrono::milliseconds Profiler::getDuration() const {
  return std::chrono::duration_cast<std::chrono::milliseconds>(endTime -
                                                               startTime);
}

unsigned int Profiler::getComparisons() const { return comparisons; }
unsigned int Profiler::getMoves() const { return moves; }

std::string Profiler::getName() const { return name; }
void Profiler::setName(std::string n) { name = n; }

void Profiler::printInfo(bool withExecTime) {
  // std::cout << name << " fez " << getComparisons() << " comparacoes e "
  //           << getMoves() << " movimentacoes";
  //
  // if (withExecTime)
  //   std::cout << " em " << getDuration().count() << "ms";
  //
  // std::cout << ".\n";
  utils::prettyPrintResults(*this, nullptr, withExecTime);
}
