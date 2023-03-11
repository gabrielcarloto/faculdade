#include <iostream>

struct DataNascimento {
  int dia, mes, ano;
};

class Pessoa {
private:
  DataNascimento dataNascimento;
  int idade;
  std::string nome;

  void calculaIdade();

public:
  Pessoa(std::string nome, const int dia, const int mes, const int ano);
  int getIdade();
  std::string getNome();
  DataNascimento getDataNascimento();
  void imprimeDados();
};
