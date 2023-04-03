#include <functional>
#include <iostream>
#include <utility>
#include <vector>

class Menu {
  using FunctionType = const std::function<void(unsigned int)>;

  std::vector<std::pair<std::string, FunctionType *>> opts;

public:
  void addOption(std::string option, FunctionType &fn) {
    opts.push_back(std::make_pair(option, &fn));
  }

  void display() {
    size_t i = 0;
    for (const auto &item : opts) {
      std::cout << i + 1 << ". " << item.first << "\n";
      i++;
    }

    std::cout << "\n";

    unsigned int choice;
    std::cin >> choice;

    while (choice < 1 || choice > opts.size()) {
      std::cout << "Digite uma das alternativas.\n\n";

      size_t i = 0;
      for (const auto &item : opts) {
        std::cout << i + 1 << ". " << item.first << "\n";
        i++;
      }

      std::cin >> choice;
    }

    (*opts.at(choice - 1).second)(choice);
  }

  void clear() { opts.clear(); }
};
