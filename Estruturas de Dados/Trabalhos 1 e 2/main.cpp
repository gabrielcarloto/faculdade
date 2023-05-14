#define _DEBUG
#include "BaseList.h"
#include "List.h"
#include "Menu.cpp"
#include "Profiler.h"
#include "Vector.h"
#include "utils.h"
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <memory>

#define FILES_PATH "./arquivos/"
#define FILES_BASENAME "NomeRG"
#define FILES_EXTENSION ".txt"

template <typename T, typename I> using BaseListType = BaseList<Person *, T, I>;

template <typename T, typename I> void printToStdout(BaseListType<T, I> &list);
template <typename T, typename I>
void saveToFile(BaseListType<T, I> &list, std::string filePath);
void pushLinesToLists(Profiler *, Profiler *, std::string, List<Person *> &,
                      Vector<Person *> &);
template <typename T, typename I>
void insertList(BaseListType<T, I> &list, Profiler *profiler, std::string &name,
                unsigned int id, size_t index);
template <typename T, typename I>
void removeList(BaseListType<T, I> &list, Profiler *profiler, size_t index);

template <typename T, typename I>
void searchListName(BaseListType<T, I> &list, Profiler *profiler,
                    std::string &name) {
  size_t index;
  bool found = list.findIndex(
      [&](auto item, auto) { return item->name == name; }, index);

  if (!found) {
    std::cout << profiler->getName() << " nao encontrou o item\n";
    return;
  }

  profiler->printInfo();

  Person *fromList = list.at(index);

  std::cout << "Item encontrado no indice " << index << " do "
            << profiler->getName() << "\n";
  std::cout << "\tNome: " << fromList->name << ", RG: " << fromList->id << "\n";
}

template <typename T, typename I>
void searchListID(BaseListType<T, I> &list, Profiler *profiler,
                  unsigned int id) {
  size_t index;
  bool found =
      list.findIndex([&](auto item, auto) { return item->id == id; }, index);

  if (!found) {
    std::cout << profiler->getName() << " nao encontrou o item\n";
    return;
  }

  profiler->printInfo();

  Person *fromList = list.at(index);

  std::cout << "Item encontrado no indice " << index << " do "
            << profiler->getName() << "\n";
  std::cout << "\tNome: " << fromList->name << ", RG: " << fromList->id << "\n";
}

size_t askForIndex();
unsigned int askForID();
std::string askForName();

int main() {
  const std::string files[] = {"10", "50", "100", "1K", "10K", "1M", "100M"};
  const size_t sizes[] = {10, 50, 100, 1000, 10000, 1000000, 13000000};
  unsigned int chosenFile;
  Menu menu("Numero de itens na lista");

  for (const auto &file : files) {
    menu.addOption(file, [&chosenFile](auto c) { chosenFile = c; });
  }

  menu.display();

  const std::string fileName = std::string(FILES_BASENAME) + files[chosenFile];
  const std::string filePath =
      std::string(FILES_PATH) + fileName + std::string(FILES_EXTENSION);

  std::cout << std::string("Lendo o arquivo ") + fileName + "...\n";

  Vector<Person *> peopleVector;
  List<Person *> peopleList;
  Profiler *vectorProfiler = peopleVector.getProfiler(),
           *listProfiler = peopleList.getProfiler();

  listProfiler->setName("List (encadeada)");
  vectorProfiler->setName("Vector (sequencial)");
  peopleVector.reserve(sizes[chosenFile]);
  peopleVector.registerItemReleaseCallback();
  peopleList.registerItemReleaseCallback();

  pushLinesToLists(vectorProfiler, listProfiler, filePath, peopleList,
                   peopleVector);

  menu.clear();
  menu.setTitle("Menu");

  auto insertMenu = menu.addNestedMenu("Inserir");

  insertMenu->addOption("no inicio", [&](auto) {
    const size_t index = 0;
    std::string name = askForName();
    unsigned int id = askForID();

    insertList(peopleList, listProfiler, name, id, index);
    insertList(peopleVector, vectorProfiler, name, id, index);
  });

  insertMenu->addOption("em um indice", [&](auto) {
    const size_t index = askForIndex();
    std::string name = askForName();
    unsigned int id = askForID();

    insertList(peopleList, listProfiler, name, id, index);
    insertList(peopleVector, vectorProfiler, name, id, index);
  });

  insertMenu->addOption("no fim", [&](auto) {
    const size_t index = peopleVector.getLength();
    std::string name = askForName();
    unsigned int id = askForID();

    insertList(peopleList, listProfiler, name, id, index);
    insertList(peopleVector, vectorProfiler, name, id, index);
  });

  auto removeMenu = menu.addNestedMenu("Remover");

  removeMenu->addOption("no inicio", [&](auto) {
    const size_t index = 0;

    removeList(peopleList, listProfiler, index);
    removeList(peopleVector, vectorProfiler, index);
  });

  removeMenu->addOption("em um indice", [&](auto) {
    const size_t index = askForIndex();

    removeList(peopleList, listProfiler, index);
    removeList(peopleVector, vectorProfiler, index);
  });

  removeMenu->addOption("no fim", [&](auto) {
    const size_t index = peopleVector.getLength() - 1;

    removeList(peopleList, listProfiler, index);
    removeList(peopleVector, vectorProfiler, index);
  });

  auto searchMenu = menu.addNestedMenu("Procurar");

  searchMenu->addOption("Nome", [&](auto) {
    std::string name = askForName();

    searchListName(peopleVector, vectorProfiler, name);
    searchListName(peopleList, listProfiler, name);
  });

  searchMenu->addOption("RG", [&](auto) {
    unsigned int id = askForID();

    searchListID(peopleVector, vectorProfiler, id);
    searchListID(peopleList, listProfiler, id);
  });

  auto saveMenu = menu.addNestedMenu("Salvar no arquivo");

  saveMenu->addOption("Vector",
                      [&](auto) { saveToFile(peopleVector, filePath); });

  saveMenu->addOption("List", [&](auto) { saveToFile(peopleList, filePath); });

  auto printMenu = menu.addNestedMenu("Imprimir na tela");

  printMenu->addOption("Vector", [&](auto) { printToStdout(peopleVector); });
  printMenu->addOption("List", [&](auto) { printToStdout(peopleList); });

  menu.loopDisplay();

  return 0;
}

std::string askForName() {
  std::string name;

  std::cout << "Digite o nome: ";
  std::cin >> name;

  return name;
};

template <typename T> T askAndCheck() {
  T value;

  while (!(std::cin >> value)) {
    std::cout << "Entrada inválida. Por favor, tente novamente: ";
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  }

  return value;
}

unsigned int askForID() {
  std::cout << "Digite o RG: ";
  return askAndCheck<unsigned int>();
};

size_t askForIndex() {
  std::cout << "Digite o indice: ";
  return askAndCheck<size_t>();
};

template <typename T, typename I> void printToStdout(BaseListType<T, I> &list) {
  std::string content = "";

  list.forEach(
      [&](auto item, auto) { content += utils::personToString(item) + '\n'; });

  std::cout << content << "\n";
  list.getProfiler()->printInfo();
}

template <typename T, typename I>
void removeList(BaseListType<T, I> &list, Profiler *profiler, size_t index) {
  try {
    list.remove(index);
    profiler->printInfo();

    Person *fromList;

    try {
      fromList = list.at(index);
    } catch (...) {
      fromList = list.at(index - 1);
    }

    std::cout << "Item removido no " << profiler->getName()
              << ". O item atual neste indice agora eh \n";
    std::cout << "\tNome: " << fromList->name << ", RG: " << fromList->id
              << "\n";
  } catch (...) {
    std::cout << "O indice esta fora de alcance.\n";
  }
}

template <typename T, typename I>
void insertList(BaseListType<T, I> &list, Profiler *profiler, std::string &name,
                unsigned int id, size_t index) {
  Person *newPerson = new Person;

  newPerson->name = name;
  newPerson->id = id;

  try {
    list.insert(newPerson, index);
    profiler->printInfo();

    Person *fromList = list.at(index);

    std::cout << "Item inserido no " << profiler->getName() << ": \n";
    std::cout << "\tNome: " << fromList->name << ", RG: " << fromList->id
              << "\n";
  } catch (...) {
    std::cout << "O indice esta fora de alcance.\n";
  }
}

template <typename T, typename I>
void saveToFile(BaseListType<T, I> &list, std::string filePath) {
  std::string content = "";

  list.forEach(
      [&](auto item, auto) { content += utils::personToString(item) + '\n'; });

  utils::writeFile(filePath, content);
  list.getProfiler()->printInfo();
}

void pushLinesToLists(Profiler *vectorProfiler, Profiler *listProfiler,
                      std::string filePath, List<Person *> &peopleList,
                      Vector<Person *> &peopleVector) {
  Profiler fileReadProfiler;

  fileReadProfiler.start();
  vectorProfiler->start();
  listProfiler->start();

  unsigned int ignoredCount = 0;

  utils::readFile(filePath, [&](std::string line) {
    if (!utils::verifyString(line)) {
      ignoredCount++;
      return;
    }

    Person *parsed1 = utils::strToPerson(line), *parsed2 = new Person;

    parsed2->name = parsed1->name;
    parsed2->id = parsed1->id;

    peopleList.push(parsed1);
    peopleVector.push(parsed2);
  });

  fileReadProfiler.end();
  vectorProfiler->end();
  listProfiler->end();

  std::cout << "A leitura do arquivo levou "
            << fileReadProfiler.getDuration().count() << "ms e " << ignoredCount
            << " linhas foram ignoradas.\n";

  vectorProfiler->printInfo(false);
  listProfiler->printInfo(false);
}
