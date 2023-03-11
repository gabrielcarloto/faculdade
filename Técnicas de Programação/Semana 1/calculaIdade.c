// será convertido para cpp
#include <stdio.h>

#define ANO_ATUAL 2023
#define MES_ATUAL 3
#define DIA_ATUAL 11

typedef struct {
  int dia, mes, ano, idade;
} Pessoa;

void calculaIdade(Pessoa *pessoa, const int anoAtual, const int mesAtual,
                  const int diaAtual);
void imprimeDados(Pessoa *pessoa, char *nome);

int main(void) {
  Pessoa einstein, newton;

  einstein.dia = 14;
  einstein.mes = 03;
  einstein.ano = 1879;

  newton.dia = 04;
  newton.mes = 01;
  newton.ano = 1643;

  calculaIdade(&einstein, ANO_ATUAL, MES_ATUAL, DIA_ATUAL);
  calculaIdade(&newton, ANO_ATUAL, MES_ATUAL, DIA_ATUAL);

  imprimeDados(&einstein, "Einstein");
  imprimeDados(&newton, "Newton");

  return 0;
}

void calculaIdade(Pessoa *pessoa, const int anoAtual, const int mesAtual,
                  const int diaAtual) {
  int diffIdade = anoAtual - pessoa->ano;

  if (mesAtual < pessoa->mes ||
      mesAtual == pessoa->mes && diaAtual < pessoa->dia)
    diffIdade--;

  pessoa->idade = diffIdade;
}

void imprimeDados(Pessoa *pessoa, char *nome) {
  printf("%s nasceu em %.2d/%.2d/%.4d e possui %d anos de idade\n", nome,
         pessoa->dia, pessoa->mes, pessoa->ano, pessoa->idade);
}
