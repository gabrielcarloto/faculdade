#include "List.h"
#include <type_traits>
#define _CRTDBG_MAP_ALLOC
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

int main() {
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

  const std::string files[] = {"10", "50", "100", "1K", "10K", "1M", "100M"};
  int chosenFile;

  std::cout << "Escolha o numero de itens na lista:\n\n";

  int i = 0;

  for (const auto &file : files) {
    std::cout << i + 1 << ". " << file << "\n";
    i++;
  }

  std::cout << "\n";
  std::cin >> chosenFile;
  const std::string fileName =
                        std::string(FILES_BASENAME) + files[chosenFile - 1],
                    filePath = std::string(FILES_PATH) + fileName +
                               std::string(FILES_EXTENSION);

  std::cout << std::string("Lendo o arquivo ") + fileName + "...\n";

  Vector<Person *> peopleVector;
  List<Person *> peopleList;
  Profiler fileReadProfiler, *vectorProfiler = peopleVector.getProfiler(),
                             *listProfiler = peopleList.getProfiler();

  vectorProfiler->setName("Vector");
  listProfiler->setName("List");
  peopleList.registerItemReleaseCallback();

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

  const std::string back = "0. Sair";
  const std::string opts[] = {"Inserir", "Remover", "Procurar",
                              "Salvar no arquivo"};

  return 0;
}
