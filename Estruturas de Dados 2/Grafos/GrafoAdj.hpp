#include <algorithm>
#include <array>
#include <cstdlib>
#include <iostream>
#include <queue>
#include <vector>

template <bool Direcionado = false> class GrafoAdj {
  struct NoAdj;
  class DFS;
  class BFS;

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
  void buscaLargura() { BFS(*this); }
};

template <bool Direcionado> class GrafoAdj<Direcionado>::DFS {
  const GrafoAdj<Direcionado> &grafo;

  struct InfoVertice {
    enum Status { NAO_VISITADO, VISITADO, COMPLETO };
    enum Particao { UM, DOIS, INDEFINIDO };

    Status status = NAO_VISITADO; // igual as cores
    Particao particao = INDEFINIDO;
    NoAdj *pai = nullptr;

    unsigned int momentoVisitado = 0;
    unsigned int momentoCompleto = 0;
  };

  std::vector<std::vector<int>> componentes;
  std::vector<InfoVertice> vertices;
  unsigned int tempo = 0;
  bool bipartido = true;

  void buscaProfundidade() {
    for (int i = 0; i < grafo.nos.size(); i++) {
      if (vertices[i].status == InfoVertice::Status::NAO_VISITADO) {
        componentes.emplace_back();
        dfs_visita(i);
      }
    }
  }

  void dfs_visita(int no, int tempo = 0, int part = InfoVertice::Particao::UM) {
    componentes[componentes.size() - 1].push_back(no);
    InfoVertice &vertice = vertices[no];
    NoAdj *no_ptr = grafo.nos[no];

    vertice.status = InfoVertice::Status::VISITADO;
    vertice.momentoVisitado = ++tempo;
    vertice.particao = (typename InfoVertice::Particao)part;

    part = part == InfoVertice::Particao::UM ? InfoVertice::Particao::DOIS
                                             : InfoVertice::Particao::UM;

    for (int i = 0; i < no_ptr->adjacencias.size(); i++) {
      Adjacencia *adj = no_ptr->adjacencias[i];
      InfoVertice &verticeAdj = vertices[adj->no->id];

      if (verticeAdj.status == InfoVertice::Status::NAO_VISITADO) {
        verticeAdj.pai = no_ptr;
        std::cout << "Aresta árvore: (" << no << " - " << adj->no->id << ")\n";
        dfs_visita(adj->no->id, tempo, part);
      } else {
        std::cout << "Aresta outra: (" << no << " - " << adj->no->id << ")\n";
      }

      if (bipartido && verticeAdj.particao == vertice.particao) {
        bipartido = false;
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

    if (bipartido) {
      std::cout << "Grafo bipartido\n";
    } else {
      std::cout << "Grafo não bipartido\n";
    }
  }

  friend GrafoAdj;
};

template <bool D> class GrafoAdj<D>::BFS {
  struct InfoVertice {
    enum Status { NAO_VISITADO, VISITADO, COMPLETO };

    Status status = NAO_VISITADO;
    unsigned int distancia = 0;
    NoAdj *pai = nullptr;
  };

  std::vector<InfoVertice> vertices;
  const GrafoAdj<D> &grafo;
  std::queue<int> fila;

  void buscaLargura() {
    fila.push(0);

    while (!fila.empty()) {
      int no = fila.front();
      bfs_visita(no);
      fila.pop();
    }
  }

  void bfs_visita(int no) {
    auto &vertice = vertices[no];
    auto no_ptr = grafo.nos[no];
    vertice.status = InfoVertice::Status::VISITADO;

    for (auto adj : no_ptr->adjacencias) {
      NoAdj *noAdj = adj->no;
      InfoVertice &verticeAdj = vertices[noAdj->id];

      if (verticeAdj.status == InfoVertice::NAO_VISITADO) {
        verticeAdj.status = InfoVertice::Status::VISITADO;
        verticeAdj.distancia = vertice.distancia + 1;
        verticeAdj.pai = no_ptr;
        fila.push(noAdj->id);
      }
    }

    vertice.status = InfoVertice::Status::COMPLETO;
  }

  void imprimeResultados() {
    for (int i = 0; i < vertices.size(); i++) {
      InfoVertice &vertice = vertices[i];
      NoAdj *no = grafo.nos[i];
      std::cout << i << " = ";

      if (!vertice.status == InfoVertice::Status::COMPLETO) {
        std::cout << "Não visitado";
        continue;
      }

      std::cout << "[d: ";

      if (i != 0 && vertice.distancia == 0) {
        std::cout << "inf.";
      } else {
        std::cout << vertice.distancia;
      }

      std::cout << ", pai: " << (int)(vertice.pai ? vertice.pai->id : -1)
                << "]\n";
    }
  }

  BFS(const GrafoAdj<D> &g)
      : grafo(g), vertices(g.nos.size(), (InfoVertice){}) {
    buscaLargura();
    imprimeResultados();
  };

  friend GrafoAdj;
};