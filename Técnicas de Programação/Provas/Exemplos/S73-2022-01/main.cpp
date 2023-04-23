#include <cstdlib>
#include <ctime>
#include <iostream>
#include <list>
#include <sstream>
#include <string>
using namespace std;

class Hormonio {
  static int soma;

protected:
  string nivel;
  bool estresse;

public:
  Hormonio() : nivel("indeterminado"), estresse(soma % 2 == 0) { soma++; };

  virtual ~Hormonio() = 0;

  const string getNivel() const { return nivel; }

  virtual void calcular_nivel() = 0;
};

Hormonio::~Hormonio() = default;
int Hormonio::soma(0);

class Serotonina;

class Endorfina : public Hormonio {
  float real;
  Serotonina *pSero;

public:
  Endorfina() : real((float)(rand() % 11)), pSero(nullptr){};
  ~Endorfina() override { pSero = nullptr; };

  const float getReal() const { return real; }

  void setSero(Serotonina *pS) { pSero = pS; }

  void calcular_nivel() override;
};

class Serotonina : public Hormonio {
  int inteiro;
  Endorfina *pEndo;

public:
  Serotonina() : inteiro(rand() % 11), pEndo(nullptr) {
    if (this->estresse && inteiro > 0)
      inteiro--;
  }

  ~Serotonina() override { pEndo = nullptr; }

  const int getInteiro() const { return inteiro; }

  void setEndo(Endorfina *pE) { pEndo = pE; }

  void calcular_nivel() override {
    if (!pEndo)
      return;

    float real = pEndo->getReal();

    if (inteiro >= 7 && inteiro <= 10 && real > 5)
      this->nivel = "normal";
    else if (inteiro > 4 && inteiro < 7 && real > 3)
      this->nivel = "medio";
    else if (inteiro >= 0 && inteiro <= 4 && real > 0)
      this->nivel = "baixo";
  }
};

void Endorfina::calcular_nivel() {
  if (!pSero)
    return;

  int inteiro = pSero->getInteiro();

  if (real >= 6 && real <= 10 && inteiro > 7)
    this->nivel = "normal";
  else if (real > 3 && real < 6 && inteiro > 5)
    this->nivel = "medio";
  else if (real >= 0 && real <= 3 && inteiro > 0)
    this->nivel = "baixo";
}

class Reg_Paciente {
  Endorfina nivel_endo;
  Serotonina nivel_sero;
  bool ok;

public:
  Reg_Paciente() : ok(false) {
    nivel_endo.setSero(&nivel_sero);
    nivel_sero.setEndo(&nivel_endo);

    nivel_endo.calcular_nivel();
    nivel_sero.calcular_nivel();
  }

  ~Reg_Paciente() = default;

  const bool getOk() const { return ok; }

  void auto_avaliar() {
    string nEndo = nivel_endo.getNivel(), nSero = nivel_sero.getNivel();

    if ((nEndo == "medio" || nEndo == "normal") &&
        (nSero == "medio" || nSero == "normal"))
      ok = true;
  }
};

class Diagnosticador {
  list<Reg_Paciente *> listap;
  ostringstream saida;

public:
  Diagnosticador(const int num = 10) {
    for (int i = 0; i < num; i++)
      listap.push_back(new Reg_Paciente());

    diagnosticar();
  }

  ~Diagnosticador() {
    for (auto pac : listap) {
      delete pac;
      pac = nullptr;
    }
  }

  ostringstream &getSaida() { return saida; }

  void diagnosticar() {
    int pacientesOk = 0;

    for (auto pac : listap) {
      if (!pac)
        continue;

      pac->auto_avaliar();
      if (pac->getOk())
        pacientesOk++;
    }

    saida << ((float)pacientesOk / (float)listap.size()) * 100 << "%";
  }
};

ostream &operator<<(ostream &out, Diagnosticador &diag) {
  out << diag.getSaida().str();
  return out;
}

int main() {
  srand(time(nullptr));
  Diagnosticador diag1(100);

  cout << diag1 << endl;

  return 0;
}
