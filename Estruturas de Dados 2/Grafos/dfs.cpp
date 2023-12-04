#include "GrafoAdj.hpp"

int main() {
  GrafoAdj<true> grafo;

  grafo.criarNo();
  grafo.criarNo();
  grafo.criarNo();
  grafo.criarNo();
  grafo.criarNo();
  grafo.criarNo();
  grafo.criarNo();
  grafo.criarNo();
  grafo.criarNo();
  grafo.criarNo();
  grafo.criarNo();
  grafo.criarNo();

  grafo.criarAresta(0, 1);
  grafo.criarAresta(0, 3);

  grafo.criarAresta(1, 2);

  grafo.criarAresta(2, 5);

  grafo.criarAresta(4, 1);

  grafo.criarAresta(5, 4);
  grafo.criarAresta(5, 6);

  grafo.criarAresta(6, 0);
  grafo.criarAresta(6, 3);
  grafo.criarAresta(6, 4);

  grafo.criarAresta(7, 8);

  grafo.criarAresta(9, 10);

  grafo.criarAresta(10, 11);

  grafo.criarAresta(11, 9);

  grafo.buscaProfundidade();

  return 0;
}