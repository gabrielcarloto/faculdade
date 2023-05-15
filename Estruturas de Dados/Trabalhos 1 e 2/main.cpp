#include "BaseList.h"
#include "List.h"
#include "Menu.cpp"
#include "Profiler.h"
#include "SortedList.h"
#include "Vector.h"
#include "utils.h"
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <stdexcept>

#define FILES_PATH "./arquivos/"
#define FILES_BASENAME "NomeRG"
#define FILES_EXTENSION ".txt"

#define FG_GREEN "\033[32m"
#define RESET_COLOR "\033[39;49m"

using SortedVector = SortedList<Person *, Vector<Person *>>;
using SortedLinkedList = SortedList<Person *, List<Person *>>;
template <class Derived> using SortedBaseList = SortedList<Person *, Derived>;

template <typename T, typename I> using BaseListType = BaseList<Person *, T, I>;

template <class T> void printToStdout(SortedBaseList<T> &list);
template <class T>
void saveToFile(SortedBaseList<T> &list, std::string filePath);
void pushLinesToLists(Profiler *, Profiler *, std::string, SortedLinkedList &,
                      SortedVector &);
template <class T>
void insertList(SortedBaseList<T> &list, Profiler *profiler, std::string &name,
                unsigned int id, size_t index);
template <class T>
void removeList(SortedBaseList<T> &list, Profiler *profiler, size_t index);

template <class T>
void searchListName(SortedBaseList<T> &list, Profiler *profiler,
                    std::string &name) {
  size_t index;
  bool found = list->findIndex(
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

template <class T>
void searchListID(SortedBaseList<T> &list, Profiler *profiler,
                  unsigned int id) {
  size_t index;
  bool found =
      list->findIndex([&](auto item, auto) { return item->id == id; }, index);

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

template <class T>
void binarySearchListID(SortedBaseList<T> &list, unsigned int id) {
  bool found = false;

  auto person = new Person;
  person->id = id;

  size_t index = list.searchIndex(person, &found);

  if (!found) {
    std::cout << list->getProfiler()->getName() << " nao encontrou o item\n";
    return;
  }

  list->getProfiler()->printInfo();
  utils::prettyPrintResults(list.getProfiler(), "Pesquisa", true, 2);

  utils::prettyPrintResults(index, list.at(index));
}

template <class T>
void orderedInsert(SortedBaseList<T> &list, std::string name, unsigned int id);

template <class T> void sortList(SortedBaseList<T> &list) {
  list.sort();
  list->getProfiler()->printInfo();
  utils::prettyPrintResults(list.getProfiler(), "Ordenacao", true, 2);
}

template <class T>
typename SortedBaseList<T>::ValueCompareFunctions createCompareFunctions() {
  typename SortedBaseList<T>::ValueCompareFunctions fns;

  // TODO: possivelmente isso deixa tudo muito mais lento
  // quem sabe, ao invés de chamar também o operator< de cada pessoa,
  // fazer a comparação direta seja melhor
  fns.less = [](Person *const &a, Person *const &b) { return *a < *b; };
  fns.lessEqual = [](Person *const &a, Person *const &b) -> bool {
    return *a <= *b;
  };

  fns.greater = [](Person *const &a, Person *const &b) -> bool {
    return *a > *b;
  };

  fns.greaterEqual = [](Person *const &a, Person *const &b) -> bool {
    return *a >= *b;
  };

  fns.equal = [](Person *const &a, Person *const &b) -> bool {
    return *a == *b;
  };

  return fns;
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

  auto vecFns = createCompareFunctions<Vector<Person *>>();
  auto listFns = createCompareFunctions<List<Person *>>();

  SortedVector peopleVector(SortedVector::bubbleSort, vecFns);
  SortedLinkedList peopleList(SortedLinkedList::bubbleSort, listFns);

  Profiler *listProfiler = peopleList->getProfiler();
  Profiler *vectorProfiler = peopleVector->getProfiler();

  listProfiler->setName("Encadeada");
  vectorProfiler->setName("Sequencial");
  peopleVector->reserve(sizes[chosenFile]);
  peopleVector->registerItemReleaseCallback();
  peopleList->registerItemReleaseCallback();

  pushLinesToLists(vectorProfiler, listProfiler, filePath, peopleList,
                   peopleVector);

  menu.clear();
  menu.setTitle("Menu");

  auto sortMenu = menu.addNestedMenu("Ordenar " FG_GREEN "(novo)");

  sortMenu->addOption("BubbleSort", [&](auto) {
    peopleVector.setSortFunction(SortedVector::bubbleSort);
    peopleList.setSortFunction(SortedLinkedList::bubbleSort);

    sortList(peopleVector);
    sortList(peopleList);
  });

  sortMenu->addOption("SelectionSort", [&](auto) {
    peopleVector.setSortFunction(SortedVector::selectionSort);
    peopleList.setSortFunction(SortedLinkedList::selectionSort);

    sortList(peopleVector);
    sortList(peopleList);
  });

  sortMenu->addOption("InsertionSort", [&](auto) {
    peopleVector.setSortFunction(SortedVector::insertionSort);
    peopleList.setSortFunction(SortedLinkedList::insertionSort);

    sortList(peopleVector);
    sortList(peopleList);
  });

  sortMenu->addOption("ShellSort", [&](auto) {
    peopleVector.setSortFunction(SortedVector::shellSort);
    peopleList.setSortFunction(SortedLinkedList::shellSort);

    sortList(peopleVector);
    sortList(peopleList);
  });

  auto quickSortMenu = sortMenu->addNestedMenu("QuickSort");

  quickSortMenu->addOption("Sequencial", [&](auto) {
    peopleVector.setSortFunction(SortedVector::quickSort);
    sortList(peopleVector);
  });

  quickSortMenu->addOption("Encadeada", [&](auto) {
    peopleList.setSortFunction(SortedLinkedList::quickSort);
    sortList(peopleList);
  });

  auto mergeSortMenu = sortMenu->addNestedMenu("MergeSort");

  mergeSortMenu->addOption("Sequencial", [&](auto) {
    peopleVector.setSortFunction(SortedVector::mergeSort);
    sortList(peopleVector);
  });

  mergeSortMenu->addOption("Encadeada", [&](auto) {
    peopleList.setSortFunction(SortedLinkedList::mergeSort);
    sortList(peopleList);
  });

  auto insertMenu = menu.addNestedMenu("Inserir");

  auto insertCommonLogic = [&](size_t index) {
    std::string name = askForName();
    unsigned int id = askForID();

    insertList(peopleList, listProfiler, name, id, index);
    insertList(peopleVector, vectorProfiler, name, id, index);
  };

  insertMenu->addOption("no inicio", [&](auto) { insertCommonLogic(0); });
  insertMenu->addOption("em um indice",
                        [&](auto) { insertCommonLogic(askForIndex()); });
  insertMenu->addOption(
      "no fim", [&](auto) { insertCommonLogic(peopleVector.getLength()); });

  insertMenu->addOption("ordenadamente" FG_GREEN " (novo)" RESET_COLOR,
                        [&](auto) {
                          std::string name = askForName();
                          unsigned int id = askForID();

                          orderedInsert(peopleVector, name, id);
                          orderedInsert(peopleList, name, id);
                        });

  auto removeMenu = menu.addNestedMenu("Remover");

  auto removeCommonLogic = [&](size_t index) {
    removeList(peopleList, listProfiler, index);
    removeList(peopleVector, vectorProfiler, index);
  };

  removeMenu->addOption("no inicio", [&](auto) { removeCommonLogic(0); });
  removeMenu->addOption("em um indice",
                        [&](auto) { removeCommonLogic(askForIndex()); });
  removeMenu->addOption(
      "no fim", [&](auto) { removeCommonLogic(peopleVector.getLength() - 1); });

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

  searchMenu->addOption("RG com busca binária " FG_GREEN "(novo)", [&](auto) {
    auto id = askForID();
    binarySearchListID(peopleVector, id);
    binarySearchListID(peopleList, id);
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
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
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

template <class T> void printToStdout(SortedBaseList<T> &list) {
  std::string content = "";

  list->forEach(
      [&](auto item, auto) { content += utils::personToString(item) + '\n'; });

  std::cout << content << "\n";
  list->getProfiler()->printInfo();
}

template <class T>
void removeList(SortedBaseList<T> &list, Profiler *profiler, size_t index) {
  try {
    list->remove(index);
    profiler->printInfo();

    Person *fromList;

    try {
      fromList = list.at(index);
    } catch (...) {
      fromList = list.at(index - 1);
    }

    utils::prettyPrintResults(index, fromList);
  } catch (...) {
    std::cout << "O indice esta fora de alcance.\n";
  }
}

template <class T>
void insertList(SortedBaseList<T> &list, Profiler *profiler, std::string &name,
                unsigned int id, size_t index) {
  auto *newPerson = new Person;

  newPerson->name = name;
  newPerson->id = id;

  try {
    list->insert(newPerson, index);

    profiler->printInfo();
    utils::prettyPrintResults(index, list.at(index));
  } catch (...) {
    std::cout << "O indice esta fora de alcance.\n";
  }
}

template <class T>
void saveToFile(SortedBaseList<T> &list, std::string filePath) {
  std::string content = "";

  list->forEach(
      [&](auto item, auto) { content += utils::personToString(item) + '\n'; });

  utils::writeFile(filePath, content);
  list->getProfiler()->printInfo();
}

void pushLinesToLists(Profiler *vectorProfiler, Profiler *listProfiler,
                      std::string filePath, SortedLinkedList &peopleList,
                      SortedVector &peopleVector) {
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

    peopleList->push(parsed1);
    peopleVector->push(parsed2);
  });

  if (!peopleList->getLength() || !peopleVector.getLength())
    throw std::runtime_error("Todas as linhas foram ignoradas");

  fileReadProfiler.end();
  vectorProfiler->end();
  listProfiler->end();

  std::cout << "A leitura do arquivo levou "
            << fileReadProfiler.getDuration().count() << "ms e " << ignoredCount
            << " linhas foram ignoradas.\n";

  vectorProfiler->printInfo(false);
  listProfiler->printInfo(false);
}

template <class T>
void orderedInsert(SortedBaseList<T> &list, std::string name, unsigned int id) {
  auto newPerson = new Person;

  newPerson->name = name;
  newPerson->id = id;

  auto index = list.add(newPerson);
  list->getProfiler()->printInfo();
  utils::prettyPrintResults(list.getProfiler(), "Ordenada", true, 2);
  utils::prettyPrintResults(index, list.at(index));
}
