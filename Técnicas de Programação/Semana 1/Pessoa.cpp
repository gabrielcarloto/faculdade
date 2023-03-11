#include "Pessoa.h"

#define ANO_ATUAL 2023
#define MES_ATUAL 3
#define DIA_ATUAL 11

Pessoa::Pessoa(std::string nome, const int dia, const int mes, const int ano) {
  dataNascimento = {.dia = dia, .mes = mes, .ano = ano};
  this->nome = nome;
  calculaIdade();
}

void Pessoa::calculaIdade() {
  int diffIdade = ANO_ATUAL - dataNascimento.ano;

  if (MES_ATUAL < dataNascimento.mes ||
      MES_ATUAL == dataNascimento.mes && DIA_ATUAL < dataNascimento.dia)
    diffIdade--;

  idade = diffIdade;
}

int Pessoa::getIdade() { return idade; }
std::string Pessoa::getNome() { return nome; }
DataNascimento Pessoa::getDataNascimento() { return dataNascimento; }
