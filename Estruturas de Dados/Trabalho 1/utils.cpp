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

Vector<char *> utils::splitStr(char *str, const char *delimiter) {
  char *token = strtok(str, delimiter);
  Vector<char *> splitted;

  while (token != NULL) {
    splitted.push(token);
    token = strtok(NULL, delimiter);
  }

  return splitted;
}

Person *utils::strToPerson(std::string &str) {
  auto splitted = splitStr(const_cast<char *>(str.data()), ",");
  Person *p = new Person;

  p->name = std::string(splitted[0]);
  p->id = std::atoi(splitted[1]);

  return p;
}

bool utils::verifyString(std::string &str) {
  size_t commaPos = str.find(',');

  if (commaPos == std::string::npos ||
      str.find(',', commaPos + 1) != std::string::npos)
    return false;

  std::string part1 = str.substr(0, commaPos);
  std::string part2 = str.substr(commaPos + 1);

  for (char c : part1) {
    if (!std::isalpha(c))
      return false;
  }

  if (part2.length() < 6 || part2.length() > 8)
    return false;

  for (char c : part2) {
    if (!std::isdigit(c))
      return false;
  }

  return true;
}
