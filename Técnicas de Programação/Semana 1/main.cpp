#include "Pessoa.h"

int main(int argc, char *argv[]) {
  Pessoa einstein("Einstein", 14, 3, 1879), newton("Newton", 4, 1, 1643);

  einstein.imprimeDados();
  newton.imprimeDados();

  return 0;
}
