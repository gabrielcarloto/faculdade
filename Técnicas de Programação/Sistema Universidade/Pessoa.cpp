#include "Pessoa.h"
#include <array>

#define ANO_ATUAL 2023
#define MES_ATUAL 3
#define DIA_ATUAL 11

Pessoa::Pessoa(std::string nome, DataNascimento &nascimento, std::string uuid)
    : Base(nome, uuid), dataNascimento(nascimento) {
  calculaIdade();
}

void Pessoa::calculaIdade() {
  int diffIdade = ANO_ATUAL - dataNascimento.ano;

  if (MES_ATUAL < dataNascimento.mes ||
      (MES_ATUAL == dataNascimento.mes && DIA_ATUAL < dataNascimento.dia))
    diffIdade--;

  idade = diffIdade;
}

auto Pessoa::getIdade() -> int { return idade; }
auto Pessoa::getDataNascimento() -> DataNascimento { return dataNascimento; }

void Pessoa::imprimeDados() {
  std::array<char, 150> buf;

  sprintf(buf.data(), "%s nasceu em %.2d/%.2d/%.4d e possui %d anos de idade",
          nome.c_str(), dataNascimento.dia, dataNascimento.mes,
          dataNascimento.ano, idade);

  std::cout << std::string(buf.data()) << '\n';
}

void Pessoa::imprimeProventos() {
  if (salario != 0)
    std::cout << "O salario de " << this->nome << " é " << salario << '\n';
  else
    std::cout << this->nome << " nao possui proventos\n";
}

void Pessoa::setProventos(double p) { salario = p; }

auto Pessoa::getProventos() -> double { return salario; }
