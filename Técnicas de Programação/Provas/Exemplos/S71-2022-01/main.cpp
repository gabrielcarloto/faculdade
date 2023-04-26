#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>

using namespace std;

class Sensor {
  static vector<Sensor *> colecao_sensores;

protected:
  static int cont;
  bool ativo;

public:
  Sensor() : ativo(rand() % 10 != 0) {
    incluirSensor(this);
    cont++;
  }

  virtual ~Sensor() {
    ativo = false;
    int i;

    for (i = 0; i < (int)colecao_sensores.size(); i++)
      if (colecao_sensores[i] == this)
        break;

    colecao_sensores.erase(colecao_sensores.begin() + i);
  }

  const bool getAtivo() const { return ativo; }

  static const bool TodosInativos() {
    bool inativos = true;
    auto &col = colecao_sensores;

    for (int i = 0; i < (int)col.size() && inativos; i++)
      if (col[i] && col[i]->getAtivo())
        inativos = false;

    return inativos;
  }

  static void incluirSensor(Sensor *pS) { colecao_sensores.push_back(pS); }

  static void percorrer() {
    for (auto sen : colecao_sensores)
      if (sen)
        sen->operator++();
  }

  virtual void operator++() = 0;
};

vector<Sensor *> Sensor::colecao_sensores;
int Sensor::cont(0);

class Temper : public Sensor {
  int temperatura;

public:
  Temper() : temperatura(cont) {}

  ~Temper() { temperatura = 0; }

  void operator++() { temperatura++; }

  const int getTemperatura() { return temperatura; }
};

class Presen : public Sensor {
  Temper *pTemper;
  bool alguem;

public:
  Presen() : pTemper(new Temper()), alguem(cont % 2 == 0) {}

  ~Presen() {
    delete pTemper;
    pTemper = nullptr;
    alguem = false;
  }

  const bool getAlguem() const { return alguem; }

  const int getTemperatura() const { return pTemper->getTemperatura(); }

  const bool getAmbosAtivos() const {
    return pTemper && ativo && pTemper->getAtivo();
  }

  void operator++() { alguem = !alguem; }
};

class Controle_Predio;

class Controle_Andar {
  Presen sens_pres;
  Controle_Predio *pCtrlPred;
  float oxigenio;
  bool incendio;

public:
  Controle_Andar(float O = 100)
      : pCtrlPred(nullptr), oxigenio(O), incendio(false) {}

  ~Controle_Andar() { pCtrlPred = nullptr; }

  void setControlePredio(Controle_Predio *pCP) { pCtrlPred = pCP; }

  const float getOxigenio() const { return oxigenio; }

  const bool getIncendio() const { return incendio; }

  void controlar();
};

class Controle_Predio {
  int temperatura;
  Controle_Andar andares[5];

public:
  Controle_Predio(int T = 0) : temperatura(T) {
    for (auto &andar : andares)
      andar.setControlePredio(this);

    executar();
  }

  ~Controle_Predio() {}

  int getTemperatura() const { return temperatura; }

  void informar_Colapso() const {
    if (Sensor::TodosInativos())
      cout << "Colapso\n";
    else {
      cout << "Sem colapso\n";
      informar_Incendio();
    }
  }

  void informar_Incendio() const;
  void executar();
};

void Controle_Andar::controlar() {
  bool ativos = sens_pres.getAmbosAtivos(),
       tempInt = sens_pres.getTemperatura() > 55,
       tempExt = tempInt >= pCtrlPred->getTemperatura(),
       alguem = sens_pres.getAlguem();

  if (ativos && tempInt && tempExt && alguem) {
    incendio = true;
  } else if (ativos && tempInt && !alguem) {
    oxigenio = 0;
    incendio = false;
  }
}

void Controle_Predio::informar_Incendio() const {
  int i = 0, cont = 0;

  for (auto &andar : andares) {
    if (andar.getIncendio()) {
      cout << "Andar " << i << " em incendio\n";
      cont++;
    }

    i++;
  }

  cout << cont << " andares em incendio\n";
}

void Controle_Predio::executar() {
  for (int i = 0; i < temperatura; i++) {
    Sensor::percorrer();

    for (auto &andar : andares)
      andar.controlar();
  }

  informar_Colapso();
}

int main() {
  srand(time(nullptr));

  Controle_Predio CP1(1), CP2(30), CP3(61);

  return 0;
}
