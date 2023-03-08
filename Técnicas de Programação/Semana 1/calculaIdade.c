// será convertido para cpp
#include <stdio.h>

#define ANO_ATUAL 2023

typedef struct {
  int dia, mes, ano, idade;
} Pessoa;

int calculaIdade(Pessoa *pessoa, int ano);
void imprimeDados(Pessoa *pessoa, char *nome);

int main(void) {
  Pessoa einstein, newton;

  einstein.dia = 14;
  einstein.mes = 03;
  einstein.ano = 1879;

  newton.dia = 04;
  newton.mes = 01;
  newton.ano = 1643;

  einstein.idade = calculaIdade(&einstein, ANO_ATUAL);
  newton.idade = calculaIdade(&newton, ANO_ATUAL);

  imprimeDados(&einstein, "Einstein");
  imprimeDados(&newton, "Newton");

  return 0;
}

int calculaIdade(Pessoa *pessoa, int anoAtual) {
  return anoAtual - pessoa->ano;
}

void imprimeDados(Pessoa *pessoa, char *nome) {
  printf("%s nasceu em %.2d/%.2d/%.4d e possui %d anos de idade\n", nome,
         pessoa->dia, pessoa->mes, pessoa->ano, pessoa->idade);
}
