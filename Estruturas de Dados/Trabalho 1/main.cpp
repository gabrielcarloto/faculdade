#define _CRTDBG_MAP_ALLOC
#include "Vector.h"
#include "utils.h"
#include <crtdbg.h>
#include <iostream>
#include <memory>
#include <stdio.h>
#include <utility>

template <typename T> void mockItems(Vector<T> &array, size_t length);

void test() {
  Vector<int *> plau;
  plau.registerItemReleaseCallback();

  std::cout << "Antes de pushar" << std::endl;
  std::cout << "Length: " << plau.getLength() << std::endl;
  std::cout << "Capacity: " << plau.getCapacity() << std::endl;

  mockItems(plau, 510);

  std::cout << "Depois de pushar" << std::endl;
  std::cout << "Length: " << plau.getLength() << std::endl;
  std::cout << "Capacity: " << plau.getCapacity() << std::endl;

  std::cout << "Indice 500: " << *plau.at(500) << std::endl;
  std::cout << "Removendo o indice 500" << std::endl;

  plau.remove(500);

  std::cout << "Depois de remover" << std::endl;
  std::cout << "Length: " << plau.getLength() << std::endl;
  std::cout << "Capacity: " << plau.getCapacity() << std::endl;

  plau.forEach([](auto item, auto i) {
    std::cout << "Item: " << *item << ", index: " << i << std::endl;
  });
}

int main() {
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
  test();

  // cout << "\nItens" << endl;
  //
  // plau.forEach([](char item, size_t) { cout << item << endl; });

  // _CrtDumpMemoryLeaks();
  return 0;
}

template <typename T> void mockItems(Vector<T> &array, size_t length) {
  for (size_t i = 0; i < length; i++) {
    // std::shared_ptr<int> plau(new int(i));
    array.push(new int(i));
  }
}
