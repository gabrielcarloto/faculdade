#include "Pessoa.h"

// NOTE: quando tento passar `dataNascimento` por referência implícita, ao
// chamar a função recebo o erro "clang: No matching function for call to
// 'imprimeDados'"
void imprimeDados(std::string nome, DataNascimento dataNascimento,
                  const int idade);

int main(int argc, char *argv[]) {
  Pessoa einstein("Einstein", 14, 3, 1879), newton("Newton", 4, 1, 1643);

  imprimeDados(einstein.getNome(), einstein.getDataNascimento(),
               einstein.getIdade());
  imprimeDados(newton.getNome(), newton.getDataNascimento(), newton.getIdade());

  return 0;
}

void imprimeDados(std::string nome, DataNascimento dataNascimento,
                  const int idade) {
  printf("%s nasceu em %.2d/%.2d/%.4d e possui %d anos de idade\n",
         nome.c_str(), dataNascimento.dia, dataNascimento.mes,
         dataNascimento.ano, idade);
}
