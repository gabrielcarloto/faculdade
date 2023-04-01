#include "utils.h"
#include <fstream>
#include <string>

void utils::readFile(
    std::string path,
    const std::function<void(std::string)> &forEachLineCallback) {
  std::string line;
  std::ifstream file(path);

  if (!file.is_open()) {
    throw std::runtime_error("Unable to open file");
  }

  while (getline(file, line))
    forEachLineCallback(line);

  file.close();
}
