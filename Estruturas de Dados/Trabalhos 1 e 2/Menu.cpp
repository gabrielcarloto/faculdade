#include "Menu.h"
#include <memory>
#include <utility>
#include <vector>

constexpr auto MENU_TITLE_COLOR = "\033[30;47m";
constexpr auto MENU_OPT_NUM_COLOR = "\033[90m";
constexpr auto RESET_CONSOLE = "\033[0m";

void Menu::addBasicOptions() {
  if (nestedLevel > 0)
    addOption("Voltar", [](auto) { return; });

  addOption("Sair", [](auto) { std::exit(0); });

  alreadyAddedBasicOptions = true;
}

std::string Menu::generateBreadcrumbs() {
  std::vector<std::string> titles;
  Menu *prevMenu = menuToReturn;

  while (prevMenu != NULL) {
    titles.push_back(prevMenu->title);
    prevMenu = prevMenu->menuToReturn;
  }

  std::string breadcrumb = "";

  for (intmax_t i = static_cast<intmax_t>(titles.size()) - 1; i >= 0; i--) {
    breadcrumb += titles[i] + " > ";
  }

  breadcrumb += title;

  return breadcrumb;
}

inline unsigned int Menu::printOptionsAndGetChoice() {
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

void Menu::addOption(std::string option, FunctionType &fn) {
  opts.emplace_back(option, std::move(fn));
}

void Menu::_display() {
  unsigned int choice = printOptionsAndGetChoice();

  while (choice >= opts.size()) {
    std::cout << "Esta opcao nao existe.\n\n";
    choice = printOptionsAndGetChoice();
  }

  return opts[choice].second(choice);
}

std::unique_ptr<Menu> Menu::addNestedMenu(std::string option) {
  std::unique_ptr<Menu> menu(new Menu(option, this, nestedLevel + 1));
  addOption(option, [&menu](auto) { menu->display(); });

  return menu;
}

void Menu::setTitle(std::string newTitle) { title = newTitle; }

void Menu::display() {
  if (!alreadyAddedBasicOptions)
    addBasicOptions();

  _display();
}

void Menu::loopDisplay() {
  continueLooping = true;

  if (!alreadyAddedBasicOptions)
    addBasicOptions();

  while (continueLooping) {
    _display();
  }
}

void Menu::stopLoop() { stopChainedLoop(); }

void Menu::clear() {
  opts.clear();
  alreadyAddedBasicOptions = false;
}
