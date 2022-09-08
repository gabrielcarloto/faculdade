#include <stdio.h>

int main()
{
  int num, ultimoDigito;

  printf("Digite um numero inteiro: ");
  scanf("%d", &num);

  ultimoDigito = num % 10;

  printf("O ultimo digito eh %d.\n", ultimoDigito);

  return 0;
}