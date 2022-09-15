#include <stdio.h>

int main()
{
  int i, num, somaPares = 0, somaImpares = 0;

  printf("Digite um inteiro: ");
  scanf("%d", &num);

  for (i = 2; i < num; i++)
  {
    if (i % 2 == 1)
      continue;

    somaPares += i;
  }

  for (i = 1; i < num; i++)
  {
    if (i % 2 == 0)
      continue;

    somaImpares += i;
  }

  printf("A soma dos numeros impares menores que %d eh %d, e a soma dos pares eh %d.\n", num, somaImpares, somaPares);

  return 0;
}