#include "utils.h"
#include <iostream>
using namespace std;

int main(int argc, char *argv[]) {
  readFile("./arquivos/NomeRG10.txt", [](string str) { cout << str << endl; });

  return 0;
}
