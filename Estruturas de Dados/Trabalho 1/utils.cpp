#include "utils.h"
#include <fstream>
#include <string>

void utils::readFile(
    std::string path,
    const std::function<void(std::string)> &forEachLineCallback) {
  std::string line;
  std::ifstream file(path);

  if (!file.is_open()) {
    std::cout << "Unable to open file" << std::endl;
    exit(1);
  }

  while (getline(file, line))
    forEachLineCallback(line);

  file.close();
}
