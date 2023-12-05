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

  grafo.criarAresta(0, 7);
  grafo.criarAresta(7, 0);
  grafo.criarAresta(0, 5);
  grafo.criarAresta(5, 0);
  grafo.criarAresta(1, 2);
  grafo.criarAresta(2, 1);
  grafo.criarAresta(1, 4);
  grafo.criarAresta(4, 1);
  grafo.criarAresta(1, 5);
  grafo.criarAresta(5, 1);
  grafo.criarAresta(2, 3);
  grafo.criarAresta(3, 2);
  grafo.criarAresta(2, 4);
  grafo.criarAresta(4, 2);
  grafo.criarAresta(3, 4);
  grafo.criarAresta(4, 3);
  grafo.criarAresta(4, 5);
  grafo.criarAresta(5, 4);
  grafo.criarAresta(6, 7);
  grafo.criarAresta(7, 6);

  grafo.buscaLargura();

  return 0;
}