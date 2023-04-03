#include <functional>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>

class Menu {
  using FunctionType = const std::function<void(unsigned int)>;

  std::vector<std::pair<std::string, FunctionType>> opts;

  unsigned int nestedNevel = 0;
  Menu *menuToReturn = NULL;

  void addBasicOptions() {
    addOption("Sair", [](auto) { return; });

    if (nestedNevel > 0)
      addOption("Voltar", [this](auto) { menuToReturn->display(); });
  }

public:
  Menu() { addBasicOptions(); }

  void addOption(std::string option, FunctionType &fn) {
    opts.push_back(std::make_pair(option, std::move(fn)));
  }

  std::unique_ptr<Menu> addNestedMenu(std::string option) {
    std::unique_ptr<Menu> menu(new Menu);

    menu->menuToReturn = this;
    menu->nestedNevel = nestedNevel + 1;

    this->addOption(option, [&menu](auto) { menu->display(); });

    return menu;
  }

  void display() {
    size_t i = 0;
    for (const auto &item : opts) {
      std::cout << i << ". " << item.first << "\n";
      i++;
    }

    std::cout << "\n";

    unsigned int choice;
    std::cin >> choice;

    while (choice >= opts.size()) {
      std::cout << "Digite uma das alternativas.\n\n";

      size_t i = 0;
      for (const auto &item : opts) {
        std::cout << i << ". " << item.first << "\n";
        i++;
      }

      std::cin >> choice;
    }

    opts[choice].second(choice);
  }

  void clear() {
    opts.clear();
    addBasicOptions();
  }
};
