#pragma once
#include "Base.h"
#include <iostream>
#include <memory>

struct DataNascimento {
  int dia, mes, ano;
};

class Pessoa : public Base {
private:
  const DataNascimento dataNascimento;
  double salario = 0;
  int idade;

  void calculaIdade();

public:
  Pessoa(const std::string, DataNascimento &, std::string = utils::get_uuid());

  auto getIdade() -> int;
  auto getDataNascimento() -> DataNascimento;

  virtual void imprimeDados();
  virtual void imprimeProventos();
  void setProventos(double);
  auto getProventos() -> double;
};

template <class C> class Trabalhador : public Pessoa {
  std::shared_ptr<C> localTrabalho;

public:
  Trabalhador(const std::string nome, DataNascimento &nascimento,
              std::string uuid = utils::get_uuid())
      : Pessoa(nome, nascimento, uuid){};

  Trabalhador(std::shared_ptr<C> trabalho, const std::string nome,
              DataNascimento &nascimento, std::string uuid = utils::get_uuid())
      : Pessoa(nome, nascimento, uuid), localTrabalho(trabalho){};

  Trabalhador(std::shared_ptr<C> trabalho, Pessoa &p)
      : Pessoa(p), localTrabalho(trabalho){};

  auto getLocalTrabalho() -> std::shared_ptr<C> { return localTrabalho; }

  template <class NC> void setLocalTrabalho(std::shared_ptr<NC> novoLocal) {
    localTrabalho = novoLocal;
  }
};
