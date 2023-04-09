#include <functional>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>

constexpr auto MENU_TITLE_COLOR = "\033[30;47m";
constexpr auto MENU_OPT_NUM_COLOR = "\033[90m";
constexpr auto RESET_CONSOLE = "\033[0m";

class Menu {
  using FunctionType = const std::function<void(unsigned int)>;

  std::vector<std::pair<std::string, FunctionType>> opts;
  bool alreadyAddedBasicOptions = false;
  bool continueLooping = false;

  const unsigned int nestedLevel = 0;
  Menu *menuToReturn = NULL;
  std::string title;

  void addBasicOptions() {
    if (nestedLevel > 0)
      addOption("Voltar", [](auto) { return; });

    addOption("Sair", [](auto) { std::exit(0); });

    alreadyAddedBasicOptions = true;
  }

  std::string generateBreadcrumbs() {
    std::vector<std::string> titles;
    Menu *prevMenu = menuToReturn;

    while (prevMenu != NULL) {
      titles.push_back(prevMenu->title);
      prevMenu = prevMenu->menuToReturn;
    }

    std::string breadcrumb = "";

    for (intmax_t i = titles.size() - 1; i >= 0; i--) {
      breadcrumb += titles[i] + " > ";
    }

    breadcrumb += title;

    return breadcrumb;
  }

  inline unsigned int printOptionsAndGetChoice() {
    std::cout << MENU_TITLE_COLOR << " " << generateBreadcrumbs() << " "
              << RESET_CONSOLE << "\n\n";

    size_t i = 0;
    for (const auto &item : opts) {
      if (i == opts.size() - (1 + !!nestedLevel))
        std::cout << "\n";

      std::cout << MENU_OPT_NUM_COLOR << i + 1 << ". " << RESET_CONSOLE
                << item.first << "\n";

      i++;
    }

    std::cout << "\nInsira uma das alternativas: ";

    unsigned int choice;
    std::cin >> choice;
    choice--;

    return choice;
  }

  inline void _display() {
    unsigned int choice = printOptionsAndGetChoice();

    while (choice >= opts.size()) {
      std::cout << "Esta opcao nao existe.\n\n";
      choice = printOptionsAndGetChoice();
    }

    return opts[choice].second(choice);
  }

  void stopChainedLoop() {
    continueLooping = false;
    Menu *prevMenu = menuToReturn;

    while (prevMenu != NULL)
      prevMenu->continueLooping = false;
  }

  Menu(std::string name, Menu *parent, unsigned int level)
      : nestedLevel(level), menuToReturn(parent), title(name){};

public:
  Menu(std::string name = "") : title(name){};

  void addOption(std::string option, FunctionType &fn) {
    opts.push_back(std::make_pair(option, std::move(fn)));
  }

  std::unique_ptr<Menu> addNestedMenu(std::string option) {
    std::unique_ptr<Menu> menu(new Menu(option, this, nestedLevel + 1));
    addOption(option, [&menu](auto) { menu->display(); });

    return menu;
  }

  void setTitle(std::string newTitle) { title = newTitle; }

  void display() {
    if (!alreadyAddedBasicOptions)
      addBasicOptions();

    _display();
  }

  void loopDisplay() {
    continueLooping = true;

    if (!alreadyAddedBasicOptions)
      addBasicOptions();

    while (continueLooping) {
      _display();
    }
  }

  void stopLoop() { stopChainedLoop(); }

  void clear() {
    opts.clear();
    alreadyAddedBasicOptions = false;
  }
};
