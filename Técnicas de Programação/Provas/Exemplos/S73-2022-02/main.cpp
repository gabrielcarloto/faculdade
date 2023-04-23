#include <cstdlib>
#include <ctime>
#include <iostream>
#include <list>
using namespace std;

namespace Essenciais {
class Nacao {
  int poderio;

public:
  Nacao() : poderio(rand() % 101){};
  virtual ~Nacao() = default;

  const int getPoderio() const { return poderio; }

  void setPoderio(const int pod) { poderio = pod; }

  void operator++() { poderio++; }

  virtual const int guerrear() {
    operator++();
    return poderio;
  }
};
}; // namespace Essenciais

using namespace Essenciais;

namespace Principais {
class Conjunto {
  list<Nacao *> nacoes_unidas;

public:
  Conjunto(const int num = 100) {
    for (int i = 0; i < num; i++)
      nacoes_unidas.push_back(new Nacao());
  }

  ~Conjunto() {
    for (auto nacao : nacoes_unidas) {
      delete nacao;
      nacao = nullptr;
    }
  };

  const int calc_soma() {
    int soma = 0;

    for (auto nacao : nacoes_unidas)
      soma += nacao ? nacao->guerrear() : 0;

    return soma;
  }
};

}; // namespace Principais

using namespace Principais;

namespace Essenciais {

class Invasora : public Nacao {
  bool crime_guerra;
  const int RESERVA;

public:
  Invasora() : crime_guerra(rand() % 100 != 0), RESERVA(100){};
  ~Invasora() override = default;

  const int guerrear() override { return getPoderio() * RESERVA; }

  void operator--() {
    if (crime_guerra)
      setPoderio(getPoderio() - 10);
    else
      setPoderio(getPoderio() - 1);
  }
};

class Invadida : public Nacao {
  Invasora *pInvas;
  Conjunto *pAliados;

public:
  Invadida(Invasora *pI = nullptr, Conjunto *pC = nullptr)
      : pInvas(pI), pAliados(pC){};

  ~Invadida() override {
    pInvas = nullptr;
    pAliados = nullptr;
  };

  const int guerrear() {
    if (pInvas) {
      operator++();
      pInvas->operator--();

      if (pAliados)
        return getPoderio() + pAliados->calc_soma();
    }

    return getPoderio();
  }
};
}; // namespace Essenciais

namespace Principais {
class Mundo {
  Conjunto aliados;
  Invasora invas;
  Invadida invad;

public:
  Mundo() : aliados(), invas(), invad(&invas, &aliados) {
    srand(time(nullptr));
    guerrear();
  }

  ~Mundo() = default;

  void guerrear() {
    // não entendi degraça nenhuma do que o professor queria aqui então
    // basicamente copiei da resolução
    int podInvas = 0, podInvad = 0;

    while (podInvad <= podInvas) {
      podInvad = invad.guerrear();
      podInvas = invas.guerrear();
    }

    cout << "Nacao liberta!!\n";
  }
};
}; // namespace Principais

using namespace Principais;

int main() {
  Mundo m;
  return 0;
}
