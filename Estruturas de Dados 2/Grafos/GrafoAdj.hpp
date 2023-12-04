#include <algorithm>
#include <array>
#include <cstdlib>
#include <iostream>
#include <vector>

template <bool Direcionado = false> class GrafoAdj {
  struct NoAdj;
  class DFS;

  struct Adjacencia {
    int peso;
    NoAdj *no;
  };

  struct NoAdj {
    size_t id;
    std::vector<Adjacencia *> adjacencias;
  };

  std::vector<NoAdj *> nos;
  int arestas = 0;

  Adjacencia *criarAdjacencia(NoAdj *no, int peso) {
    return new Adjacencia{.peso = peso, .no = no};
  }

public:
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
    nos.push_back(no);
    return no;
  }

  void criarAresta(size_t indice_a, size_t indice_b, int peso = 0) {
    if (std::max(indice_a, indice_b) > nos.size() - 1) {
      std::cout << "GrafoAdj::criarAresta - Índice fornecido excede o número "
                   "de nós atual.\nÍndice A: "
                << indice_a << "; Índice B: " << indice_b
                << "; Nós: " << nos.size() << '\n';
      std::exit(1);
    }

    NoAdj *a = nos[indice_a], *b = nos[indice_b];
    criarAresta(a, b, peso);
  }

  void criarAresta(NoAdj *a, NoAdj *b, int peso = 0) {
    a->adjacencias.push_back(criarAdjacencia(b, peso));

    if constexpr (!Direcionado)
      b->adjacencias.push_back(criarAdjacencia(b, peso));

    arestas++;
  }

  void buscaProfundidade() { DFS(*this); }
};

template <bool Direcionado> class GrafoAdj<Direcionado>::DFS {
  const GrafoAdj<Direcionado> &grafo;

  struct InfoVertice {
    enum Status { NAO_VISITADO, VISITADO, COMPLETO };

    Status status = NAO_VISITADO; // igual as cores
    NoAdj *pai = nullptr;

    unsigned int momentoVisitado = 0;
    unsigned int momentoCompleto = 0;
  };

  std::vector<std::vector<int>> componentes;
  std::vector<InfoVertice> vertices;
  unsigned int tempo = 0;

  void buscaProfundidade() {
    for (int i = 0; i < grafo.nos.size(); i++) {
      if (vertices[i].status == InfoVertice::Status::NAO_VISITADO) {
        componentes.emplace_back();
        dfs_visita(i);
      }
    }
  }

  void dfs_visita(int no, int tempo = 0) {
    componentes[componentes.size() - 1].push_back(no);
    InfoVertice &vertice = vertices[no];
    NoAdj *no_ptr = grafo.nos[no];

    vertice.status = InfoVertice::Status::VISITADO;
    vertice.momentoVisitado = ++tempo;

    for (int i = 0; i < no_ptr->adjacencias.size(); i++) {
      Adjacencia *adj = no_ptr->adjacencias[i];
      InfoVertice &verticeAdj = vertices[adj->no->id];

      if (verticeAdj.status == InfoVertice::Status::NAO_VISITADO) {
        verticeAdj.pai = no_ptr;
        std::cout << "Aresta árvore: (" << no << " - " << adj->no->id << ")\n";
        dfs_visita(adj->no->id, tempo);
      } else {
        std::cout << "Aresta outra: (" << no << " - " << adj->no->id << ")\n";
      }
    }

    vertice.status = InfoVertice::Status::COMPLETO;
    vertice.momentoCompleto = ++tempo;
  }

  void imprimirComponentes() {
    for (auto componente : componentes) {
      std::cout << "Componente: ";

      for (auto no : componente)
        std::cout << no << ' ';

      std::cout << '\n';
    }
  }

  DFS(const GrafoAdj<Direcionado> &g)
      : grafo(g), vertices(g.nos.size(), (InfoVertice){}) {
    buscaProfundidade();
    imprimirComponentes();
  }

  friend GrafoAdj;
};