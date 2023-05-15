#include "utils.h"
#include "Vector.h"
#include <fstream>
#include <string.h>
#include <string>
#include <string_view>

#define BG_GREEN "\033[30;42m"
#define BG_RED "\033[30;41m"
#define BG_YELLOW "\033[30;43m"
#define BG_BLUE "\033[30;44m"

#define FG_GREEN "\033[32m"
#define FG_GRAY "\033[90m"

#define RESET_COLOR "\033[39;49m"

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

void utils::writeFile(std::string path, std::string content) {
  std::ofstream file(path);

  if (!file.is_open())
    throw std::runtime_error("Unable to open file");

  file << content;
  file.close();
}

Vector<char *> utils::splitStr(char *str, const char *delimiter,
                               size_t reserve) {
  char *token = strtok(str, delimiter);
  Vector<char *> splitted;

  if (reserve)
    splitted.reserve(reserve);

  while (token != NULL) {
    splitted.push(token);
    token = strtok(NULL, delimiter);
  }

  return splitted;
}

Person *utils::strToPerson(std::string &str) {
  auto splitted = splitStr(str.data(), ",", 2);
  Person *p = new Person;

  p->name = splitted[0];
  p->id = std::atoi(splitted[1]);

  return p;
}

std::string utils::personToString(Person *p) {
  return "Nome: " + std::string(p->name) + ", RG: " + std::to_string(p->id);
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

void printListItem(const std::string &str, size_t indentationLevel = 1) {
  std::string indent(2 * indentationLevel, ' ');
  std::cout << indent << FG_GRAY << "- " RESET_COLOR << str << '\n';
}

// clang-format off
void utils::prettyPrintResults(const Profiler &prof, const char *name, bool withExecTime, size_t indentationLevel) {
  std::cout << BG_BLUE << " " << (name ? name : prof.getName()) << " " << RESET_COLOR "\n";
  
  if (withExecTime)
    printListItem("Tempo: " + std::to_string(prof.getDuration().count()) + "ms", indentationLevel);
  
  printListItem("Comparacoes: " + std::to_string(prof.getComparisons()), indentationLevel);
  printListItem("Movimentacoes: " + std::to_string(prof.getMoves()), indentationLevel);
  // clang-format on
}

// clang-format off
void utils::prettyPrintResults(size_t index, Person *person) {
  // clang-format on
  // prettyPrintResults(prof, true);

  if (!person) {
    printListItem(BG_RED " Pessoa nao encontrada ");
    return;
  }

  printListItem(BG_GREEN " Pessoa " RESET_COLOR);
  printListItem("Nome: " + person->name, 2);
  printListItem("RG: " + std::to_string(person->id), 2);
  printListItem("Índice: " + std::to_string(index), 2);
}
