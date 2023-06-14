#include <functional>
#include <iostream>
#include <memory>

class Menu {
  using FunctionType = const std::function<void(unsigned int)>;

  std::vector<std::pair<std::string, FunctionType>> opts;
  bool alreadyAddedBasicOptions = false;
  bool continueLooping = false;

  const unsigned int nestedLevel = 0;
  Menu *menuToReturn = nullptr;
  std::string title;

  void addBasicOptions();
  std::string generateBreadcrumbs();
  inline unsigned int printOptionsAndGetChoice();
  inline void _display();
  void stopChainedLoop();
  Menu(std::string name, Menu *parent, unsigned int level)
      : nestedLevel(level), menuToReturn(parent), title(name){};

public:
  Menu(std::string name = "") : title(name){};

  void addOption(std::string option, FunctionType &fn);
  std::unique_ptr<Menu> addNestedMenu(std::string option);
  void setTitle(std::string newTitle);
  void display();
  void loopDisplay();
  void stopLoop();
  void clear();
};
