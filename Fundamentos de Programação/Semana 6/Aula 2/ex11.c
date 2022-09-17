#include <stdio.h>

int main()
{
  int n, k, i, j, ehPrimo, cont = 0;

  printf("Digite um inteiro: ");
  scanf("%d", &k);

  while (k <= 0)
  {
    printf("O numero deve ser positivo. Digite outro: ");
    scanf("%d", &k);
  }

  printf("Agora, digite quantos numeros primos voce quer a partir deste: ");
  scanf("%d", &n);

  while (n <= 0)
  {
    printf("O numero deve ser positivo. Digite outro: ");
    scanf("%d", &n);
  }

  printf("Os %d numeros primos a partir de %d sao:\n", n, k);

  for (i = k + 1; cont != n; i++)
  {
    ehPrimo = 1;

    if (i == 1)
      ehPrimo = 0;
    else
      for (j = 2; j < i && ehPrimo; j++)
        if (i % j == 0)
          ehPrimo = 0;

    if (ehPrimo)
    {
      printf("%d\n", i);
      cont++;
    }
  }

  return 0;
}