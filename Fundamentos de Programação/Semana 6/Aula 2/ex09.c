#include <stdio.h>

int main()
{
  int t, i, ehPrimo, soma = 0;

  printf("Digite numeros inteiros positivos (e um negativo quando quiser parar).\n");

  scanf("%d", &t);

  while (t > 0)
  {
    ehPrimo = 1;

    if (t == 1)
      ehPrimo = 0;
    else
      for (i = 2; i < t && ehPrimo; i++)
        if (t % i == 0)
          ehPrimo = 0;

    if (ehPrimo)
      soma += t;

    scanf("%d", &t);
  }

  printf("A soma dos numeros primos entre os que digitou eh %d.\n", soma);

  return 0;
}