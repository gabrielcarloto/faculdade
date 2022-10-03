#include <stdio.h>

int ehBissexto(int ano);

int main()
{
  int ano;

  printf("Digite um ano (saira 1 se for bissexto e 0 caso contrario): ");
  scanf("%d", &ano);

  printf("%d", ehBissexto(ano));

  return 0;
}

int ehBissexto(int ano)
{
  return (ano % 4 == 0 && (ano % 100 != 0 || ano % 400 == 0)) ? 1 : 0;
}