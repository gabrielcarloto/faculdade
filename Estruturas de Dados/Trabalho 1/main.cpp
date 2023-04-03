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

void pushLinesToLists(Profiler *, Profiler *, std::string, List<Person *> &,
                      Vector<Person *> &);

int main() {
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

  const std::string files[] = {"10", "50", "100", "1K", "10K", "1M", "100M"};
  unsigned int chosenFile;
  Menu menu;

  std::cout << "Escolha o numero de itens na lista:\n\n";

  for (const auto &file : files) {
    menu.addOption(file, [&chosenFile](auto c) { chosenFile = c; });
  }

  menu.display();

  const std::string fileName =
                        std::string(FILES_BASENAME) + files[chosenFile - 1],
                    filePath = std::string(FILES_PATH) + fileName +
                               std::string(FILES_EXTENSION);

  std::cout << std::string("Lendo o arquivo ") + fileName + "...\n";

  Vector<Person *> peopleVector; // TODO: reserve
  List<Person *> peopleList;
  Profiler *vectorProfiler = peopleVector.getProfiler(),
           *listProfiler = peopleList.getProfiler();

  vectorProfiler->setName("Vector");
  listProfiler->setName("List");
  peopleList.registerItemReleaseCallback();

  pushLinesToLists(vectorProfiler, listProfiler, filePath, peopleList,
                   peopleVector);

  const std::string opts[] = {"Inserir", "Remover", "Procurar",
                              "Salvar no arquivo"};

  return 0;
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

    Person *parsed = utils::strToPerson(line);

    peopleList.push(parsed);
    peopleVector.push(parsed);
  });
  fileReadProfiler.end();
  vectorProfiler->end();
  listProfiler->end();

  std::cout << "A leitura do arquivo levou "
            << fileReadProfiler.getDuration().count() << "ms.\n";

  vectorProfiler->printInfo(false);
  listProfiler->printInfo(false);
}
