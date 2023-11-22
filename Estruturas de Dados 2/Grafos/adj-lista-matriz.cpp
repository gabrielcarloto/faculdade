#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include <array>
#include <list>
#include <vector>

enum Direcao { DE, PARA, BIDIRECIONAL };

struct NoAdj;

struct Adjacencia {
  Direcao dir;
  int peso;
  NoAdj *no;
};

struct NoAdj {
  size_t id;
  std::list<Adjacencia *> adjacencias;
  int peso;
};

class GrafoAdj {
  std::vector<NoAdj *> nos;
  int arestas = 0;
  const bool direcionado = false;

  Adjacencia *criarAdjacencia(NoAdj *no, Direcao dir, int peso) {
    return new Adjacencia{.dir = dir, .peso = peso, .no = no};
  }

public:
  GrafoAdj(bool dir = false) : direcionado(dir) {}

  ~GrafoAdj() {
    for (auto no : nos) {
      for (auto adj : no->adjacencias)
        delete adj;

      delete no;
    }
  }

  const std::vector<NoAdj *> getNos() { return nos; }

  const NoAdj *operator[](size_t i) { return nos[i]; }

  size_t getVertices() { return nos.size(); }

  int getArestas() { return arestas; }

  const NoAdj *criarNo() {
    auto no = new NoAdj;
    no->id = nos.size();
    no->peso = 0;
    nos.push_back(no);
    return no;
  }

  void criarAresta(size_t indice_a, size_t indice_b, int peso = 0) {
    NoAdj *a = nos[indice_a], *b = nos[indice_b];
    criarAresta(a, b, peso);
  }

  void criarAresta(NoAdj *a, NoAdj *b, int peso = 0) {
    a->adjacencias.push_back(
        criarAdjacencia(b, direcionado ? PARA : BIDIRECIONAL, peso));
    b->adjacencias.push_back(
        criarAdjacencia(b, direcionado ? DE : BIDIRECIONAL, peso));

    arestas++;
  }

  void imprimirArestasIncidentes(size_t no) {
    int cont = 0;
    auto pNo = nos[no];

    for (auto adj : pNo->adjacencias) {
      if (adj->dir != PARA)
        cont++;
    }

    std::cout << "Número de arestas incidentes ao vértice " << no << ": "
              << cont << '\n';
  }

  void imprimirArestasQueSaem(size_t no) {
    int cont = 0;
    auto pNo = nos[no];

    for (auto adj : pNo->adjacencias) {
      if (adj->dir != DE)
        cont++;
    }

    std::cout << "Número de arestas que saem do vértice " << no << ": " << cont
              << '\n';
  }

  void imprimirAdjacentes(size_t no) {
    std::cout << "Vértices adjacentes ao vértice " << no << ": ";
    auto pNo = nos[no];

    for (auto adj : pNo->adjacencias) {
      if (adj->dir != DE) {
        std::cout << adj->no->id << ' ';
      }
    }

    std::cout << '\n';
  }

  void imprimir() {
    for (auto no : nos) {
      std::cout << no->id << " -> ";
      for (auto next : no->adjacencias) {
        if (next->dir == DE)
          continue;

        std::cout << next->no->id << ' ';

        if (next->peso != 0) {
          std::cout << "(w:" << next->peso << ") ";
        }
      }

      std::cout << '\n';
    }
  }
};

template <size_t Tamanho> class GrafoMatriz {
  using Matriz = std::array<std::array<int, Tamanho>, Tamanho>;

  Matriz nos;
  int arestas = 0;
  size_t num_nos = 0;
  const bool direcionado = false;

public:
  GrafoMatriz(bool dir = false) : direcionado(dir) {
    for (auto coluna : nos) {
      for (auto &linha : coluna) {
        linha = 0;
      }
    }
  }

  const Matriz getMatriz() { return nos; }
  size_t getVertices() { return num_nos; }
  int getArestas() { return arestas; }

  size_t criarNo() {
    num_nos++;
    return num_nos - 1;
  }

  void criarAresta(size_t a, size_t b, int peso = 1) {
    nos[a][b] = peso;

    if (!direcionado) {
      nos[b][a] = peso;
    }

    arestas++;
  }

  void imprimirArestasIncidentes(size_t no) {
    int cont = 0;

    for (size_t i = 0; i < num_nos; i++)
      if (nos[i][no] > 0)
        cont++;

    std::cout << "Número de arestas incidentes ao vértice " << no << ": "
              << cont << '\n';
  }

  void imprimirArestasQueSaem(size_t no) {
    int cont = 0;

    for (size_t i = 0; i < num_nos; i++)
      if (nos[no][i] > 0)
        cont++;

    std::cout << "Número de arestas que saem do vértice " << no << ": " << cont
              << '\n';
  }

  void imprimirAdjacentes(size_t no) {
    std::cout << "Vértices adjacentes ao vértice " << no << ": ";

    for (size_t i = 0; i < num_nos; i++)
      if (nos[no][i] > 0)
        std::cout << i << ' ';

    std::cout << '\n';
  }

  void imprimir() {
    std::cout << "   ";
    for (size_t i = 0; i < num_nos; i++)
      std::cout << i << ' ';

    std::cout << "\n\n";

    for (size_t i = 0; i < num_nos; i++) {
      std::cout << i << "  ";
      for (size_t j = 0; j < num_nos; j++) {
        std::cout << nos[i][j] << ' ';
      }
      std::cout << '\n';
    }
  }
};

void ex1() {
  std::cout << "Exercício 1\n\n";

  {
    std::cout << "Grafo Lista Adjacência\n\n";
    GrafoAdj grafo;

    grafo.criarNo();
    grafo.criarNo();
    grafo.criarNo();
    grafo.criarNo();
    grafo.criarNo();
    grafo.criarNo();

    grafo.criarAresta(0, 1);
    grafo.criarAresta(0, 2);
    grafo.criarAresta(0, 5);

    grafo.criarAresta(1, 2);
    grafo.criarAresta(1, 3);

    grafo.criarAresta(2, 3);
    grafo.criarAresta(2, 4);

    grafo.criarAresta(3, 4);

    grafo.criarAresta(4, 5);

    grafo.imprimir();
  }

  {
    std::cout << "\n\nGrafo Matriz\n\n";
    GrafoMatriz<10> grafo;

    grafo.criarNo();
    grafo.criarNo();
    grafo.criarNo();
    grafo.criarNo();
    grafo.criarNo();
    grafo.criarNo();

    grafo.criarAresta(0, 1);
    grafo.criarAresta(0, 2);
    grafo.criarAresta(0, 5);

    grafo.criarAresta(1, 2);
    grafo.criarAresta(1, 3);

    grafo.criarAresta(2, 3);
    grafo.criarAresta(2, 4);

    grafo.criarAresta(3, 4);

    grafo.criarAresta(4, 5);

    grafo.imprimir();
  }
}

void ex3() {
  std::cout << "\n\nExercício 3\n\n";

  {
    std::cout << "Grafo Lista Adjacência Direcionado com pesos\n\n";
    GrafoAdj grafo(true);

    grafo.criarNo();
    grafo.criarNo();
    grafo.criarNo();
    grafo.criarNo();
    grafo.criarNo();
    grafo.criarNo();

    grafo.criarAresta(0, 1, 1);
    grafo.criarAresta(0, 3, 5);

    grafo.criarAresta(1, 3, 8);

    grafo.criarAresta(2, 0, 5);
    grafo.criarAresta(2, 3, 4);

    grafo.criarAresta(3, 3, 6);
    grafo.criarAresta(3, 4, 3);

    grafo.criarAresta(4, 2, 7);
    grafo.criarAresta(4, 5, 9);

    grafo.criarAresta(5, 0, 2);

    grafo.imprimir();

    std::cout << '\n';
    grafo.imprimirArestasIncidentes(3);
    grafo.imprimirArestasQueSaem(4);
    grafo.imprimirAdjacentes(2);
  }

  {
    std::cout << "\n\nGrafo Matriz Direcionado com pesos\n\n";
    GrafoMatriz<10> grafo(true);

    grafo.criarNo();
    grafo.criarNo();
    grafo.criarNo();
    grafo.criarNo();
    grafo.criarNo();
    grafo.criarNo();

    grafo.criarAresta(0, 1, 1);
    grafo.criarAresta(0, 3, 5);

    grafo.criarAresta(1, 3, 8);

    grafo.criarAresta(2, 0, 5);
    grafo.criarAresta(2, 3, 4);

    grafo.criarAresta(3, 3, 6);
    grafo.criarAresta(3, 4, 3);

    grafo.criarAresta(4, 2, 7);
    grafo.criarAresta(4, 5, 9);

    grafo.criarAresta(5, 0, 2);

    grafo.imprimir();

    std::cout << '\n';
    grafo.imprimirArestasIncidentes(3);
    grafo.imprimirArestasQueSaem(4);
    grafo.imprimirAdjacentes(2);
  }
}

void ex2() {
  std::cout << "\n\nExercício 2\n\n";

  {
    std::cout << "Grafo Lista Adjacência Direcionado\n\n";
    GrafoAdj grafo(true);

    grafo.criarNo();
    grafo.criarNo();
    grafo.criarNo();
    grafo.criarNo();
    grafo.criarNo();
    grafo.criarNo();

    grafo.criarAresta(0, 1);
    grafo.criarAresta(0, 5);

    grafo.criarAresta(1, 2);
    grafo.criarAresta(1, 3);

    grafo.criarAresta(2, 0);
    grafo.criarAresta(2, 4);

    grafo.criarAresta(3, 2);

    grafo.criarAresta(4, 3);

    grafo.criarAresta(5, 4);

    grafo.imprimir();
  }

  {
    std::cout << "\n\nGrafo Matriz Direcionado\n\n";
    GrafoMatriz<10> grafo(true);

    grafo.criarNo();
    grafo.criarNo();
    grafo.criarNo();
    grafo.criarNo();
    grafo.criarNo();
    grafo.criarNo();

    grafo.criarAresta(0, 1);
    grafo.criarAresta(0, 5);

    grafo.criarAresta(1, 2);
    grafo.criarAresta(1, 3);

    grafo.criarAresta(2, 0);
    grafo.criarAresta(2, 4);

    grafo.criarAresta(3, 2);

    grafo.criarAresta(4, 3);

    grafo.criarAresta(5, 4);

    grafo.imprimir();
  }
}

int main() {
  ex1();
  ex2();
  ex3();
  return 0;
}