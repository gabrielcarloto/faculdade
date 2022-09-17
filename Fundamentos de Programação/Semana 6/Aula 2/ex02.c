#include <stdio.h>

int main()
{
  int m, n, i, soma;

  printf("Digite um inteiro: ");
  scanf("%d", &m);

  printf("| 1^3 = 1 | ");

  for (n = 2; n <= m; n++)
  {
    soma = 0;
    printf("%d^3 = ", n);

    for (i = n * (n - 1); i < n * n * n && soma != n * n * n; i++)
    {
      if (i % 2 == 1)
      {
        soma += i;
        printf("%d + ", i);
      }
    }

    printf("\b\b| ");
  }

  return 0;
}