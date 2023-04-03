#include "BaseList.h"
#include "List.h"
#include <type_traits>
#define _CRTDBG_MAP_ALLOC
#include "Menu.cpp"
#include "Profiler.h"
#include "Vector.h"
#include "utils.h"
#include <crtdbg.h>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <stdio.h>
#include <utility>

#define FILES_PATH "./arquivos/"
#define FILES_BASENAME "NomeRG"
#define FILES_EXTENSION ".txt"

template <typename T> void printToStdout(BaseList<Person *, T> &list);
template <typename T>
void saveToFile(BaseList<Person *, T> &list, std::string filePath);
void pushLinesToLists(Profiler *, Profiler *, std::string, List<Person *> &,
                      Vector<Person *> &);
void insertList(List<Person *> &peopleList, Vector<Person *> &peopleVector,
                size_t index, Profiler *listProfiler, Profiler *vectorProfiler);

int main() {
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

  const std::string files[] = {"10", "50", "100", "1K", "10K", "1M", "100M"};
  const size_t sizes[] = {10, 50, 100, 1000, 10000, 1000000, 12000000};
  unsigned int chosenFile;
  Menu menu;

  std::cout << "Escolha o numero de itens na lista:\n\n";

  for (const auto &file : files) {
    menu.addOption(file, [&chosenFile](auto c) { chosenFile = c; });
  }

  menu.display();

  const std::string fileName =
      std::string(FILES_BASENAME) + files[chosenFile - 1];
  const std::string filePath =
      std::string(FILES_PATH) + fileName + std::string(FILES_EXTENSION);

  std::cout << std::string("Lendo o arquivo ") + fileName + "...\n";

  Vector<Person *> peopleVector;
  List<Person *> peopleList;
  Profiler *vectorProfiler = peopleVector.getProfiler(),
           *listProfiler = peopleList.getProfiler();

  listProfiler->setName("List");
  vectorProfiler->setName("Vector");
  peopleVector.reserve(sizes[chosenFile - 1]);
  peopleVector.registerItemReleaseCallback();
  peopleList.registerItemReleaseCallback();

  pushLinesToLists(vectorProfiler, listProfiler, filePath, peopleList,
                   peopleVector);

  while (true) {
    menu.clear();

    auto insertMenu = menu.addNestedMenu("Inserir");

    insertMenu->addOption("no inicio", [&](auto) {
      std::string name;
      unsigned int id;

      std::cout << "Digite o nome: ";
      std::cin >> name;
      std::cout << "Digite o RG: ";
      std::cin >> id;

      Person *newPerson = new Person;

      newPerson->name = name;
      newPerson->id = id;

      peopleVector.insert(newPerson);
      peopleList.insert(newPerson);

      vectorProfiler->printInfo();
      listProfiler->printInfo();

      Person *fromList = peopleList.at(0), *fromVector = peopleVector.at(0);

      std::cout << "Item inserido: \n";
      std::cout << "List - nome: " << fromList->name << ", RG: " << fromList->id
                << "\n";
      std::cout << "Vector - nome: " << fromVector->name
                << ", RG: " << fromVector->id << "\n";
    });

    insertMenu->addOption("em um indice", [&](auto) {
      std::string name;
      unsigned int id;
      size_t index;

      std::cout << "Digite o indice: ";
      std::cin >> index;
      std::cout << "Digite o nome: ";
      std::cin >> name;
      std::cout << "Digite o RG: ";
      std::cin >> id;

      Person *newPerson = new Person;

      newPerson->name = name;
      newPerson->id = id;

      peopleVector.insert(newPerson, index);
      peopleList.insert(newPerson, index);

      vectorProfiler->printInfo();
      listProfiler->printInfo();

      Person *fromList = peopleList.at(index),
             *fromVector = peopleVector.at(index);

      std::cout << "Item inserido: \n";
      std::cout << "List - nome: " << fromList->name << ", RG: " << fromList->id
                << "\n";
      std::cout << "Vector - nome: " << fromVector->name
                << ", RG: " << fromVector->id << "\n";
    });

    insertMenu->addOption("no fim", [&](auto) {
      std::string name;
      unsigned int id;

      std::cout << "Digite o nome: ";
      std::cin >> name;
      std::cout << "Digite o RG: ";
      std::cin >> id;

      Person *newPerson = new Person;

      newPerson->name = name;
      newPerson->id = id;

      peopleVector.push(newPerson);
      peopleList.push(newPerson);

      vectorProfiler->printInfo();
      listProfiler->printInfo();

      Person *fromList = peopleList.at(-1), *fromVector = peopleVector.at(-1);

      std::cout << "Item inserido: \n";
      std::cout << "List - nome: " << fromList->name << ", RG: " << fromList->id
                << "\n";
      std::cout << "Vector - nome: " << fromVector->name
                << ", RG: " << fromVector->id << "\n";
    });

    auto removeMenu = menu.addNestedMenu("Remover");

    removeMenu->addOption("no inicio", [&](auto) {
      Person *fromList = peopleList.at(0), *fromVector = peopleVector.at(0);

      std::cout << "Removendo (Vector) pessoa " << fromVector->name << " RG "
                << fromVector->id << "\n";

      peopleVector.remove(0);

      std::cout << "Removendo (List) pessoa " << fromList->name << " RG "
                << fromList->id << "\n";

      peopleList.remove(0);

      vectorProfiler->printInfo();
      listProfiler->printInfo();
    });

    removeMenu->addOption("em um indice", [&](auto) {
      size_t index;

      std::cout << "Digite o indice: ";
      std::cin >> index;

      Person *fromList = peopleList.at(index),
             *fromVector = peopleVector.at(index);

      std::cout << "Removendo (Vector) pessoa " << fromVector->name << " RG "
                << fromVector->id << "\n";

      peopleVector.remove(index);

      std::cout << "Removendo (List) pessoa " << fromList->name << " RG "
                << fromList->id << "\n";

      peopleList.remove(index);

      vectorProfiler->printInfo();
      listProfiler->printInfo();
    });

    removeMenu->addOption("no fim", [&](auto) {
      Person *fromList = peopleList.at(-1), *fromVector = peopleVector.at(-1);

      std::cout << "Removendo (Vector) pessoa " << fromVector->name << " RG "
                << fromVector->id << "\n";

      size_t index = peopleVector.getLength() - 1;
      peopleVector.remove(index);

      std::cout << "Removendo (List) pessoa " << fromList->name << " RG "
                << fromList->id << "\n";

      peopleList.remove(index);

      vectorProfiler->printInfo();
      listProfiler->printInfo();
    });

    auto searchMenu = menu.addNestedMenu("Procurar");

    searchMenu->addOption("Nome", [&](auto) {
      std::string name;

      std::cout << "Digite o nome que deseja procurar: ";
      std::cin >> name;

      Person *fromList, *fromVector;
      bool foundFromVector = peopleVector.find(
          [&](auto item, auto) { return item->name == name; }, fromVector);
      bool foundFromList = peopleList.find(
          [&](auto item, auto) { return item->name == name; }, fromList);

      vectorProfiler->printInfo();
      listProfiler->printInfo();

      if (foundFromList) {
        std::cout << "List - nome: " << fromList->name
                  << ", RG: " << fromList->id << "\n";

      } else {
        std::cout << "List nao encontrou a pessoa\n";
      }

      if (foundFromVector) {
        std::cout << "Vector - nome: " << fromVector->name
                  << ", RG: " << fromVector->id << "\n";

      } else {
        std::cout << "Vector nao encontrou a pessoa\n";
      }
    });

    searchMenu->addOption("RG", [&](auto) {
      unsigned int id;

      std::cout << "Digite o RG que deseja procurar: ";
      std::cin >> id;

      Person *fromList, *fromVector;
      bool foundFromVector = peopleVector.find(
          [&](auto item, auto) { return item->id == id; }, fromVector);
      bool foundFromList = peopleList.find(
          [&](auto item, auto) { return item->id == id; }, fromList);

      vectorProfiler->printInfo();
      listProfiler->printInfo();

      if (foundFromList) {
        std::cout << "List - nome: " << fromList->name
                  << ", RG: " << fromList->id << "\n";

      } else {
        std::cout << "List nao encontrou a pessoa\n";
      }

      if (foundFromVector) {
        std::cout << "Vector - nome: " << fromVector->name
                  << ", RG: " << fromVector->id << "\n";

      } else {
        std::cout << "Vector nao encontrou a pessoa\n";
      }
    });

    auto saveMenu = menu.addNestedMenu("Salvar no arquivo");

    saveMenu->addOption("Vector",
                        [&](auto) { saveToFile(peopleVector, filePath); });

    saveMenu->addOption("List",
                        [&](auto) { saveToFile(peopleList, filePath); });

    auto printMenu = menu.addNestedMenu("Imprimir na tela");

    printMenu->addOption("Vector", [&](auto) { printToStdout(peopleVector); });
    printMenu->addOption("List", [&](auto) { printToStdout(peopleList); });

    menu.display();
  }

  return 0;
}

template <typename T> void printToStdout(BaseList<Person *, T> &list) {
  std::string content = "";

  list.forEach(
      [&](auto item, auto) { content += utils::personToString(item) + '\n'; });

  std::cout << content << "\n";
  list.getProfiler()->printInfo();
}

template <typename T>
void saveToFile(BaseList<Person *, T> &list, std::string filePath) {
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
    // regex eh muito lento meu deus
    if (!utils::verifyString(line)) {
      ignoredCount++;
      return;
    }

    Person *parsed1 = utils::strToPerson(line), *parsed2 = new Person;

    parsed2->name = std::string(parsed1->name);
    parsed2->id = parsed1->id;

    peopleList.push(parsed1);
    peopleVector.push(parsed2);
  });
  fileReadProfiler.end();
  vectorProfiler->end();
  listProfiler->end();

  std::cout << "A leitura do arquivo levou "
            << fileReadProfiler.getDuration().count() << "ms.\n";

  vectorProfiler->printInfo(false);
  listProfiler->printInfo(false);
}
