#include <stdio.h>

int main()
{
  int n, x, conta;

  printf("Digite um inteiro positivo: ");
  scanf("%d", &n);

  for (x = -n; x <= n; x++)
  {
    printf("%d\t", x);

    conta = x * x + x + 1;

    for (conta; conta > 0; conta--)
      printf(".");

    printf("*\n");
  }

  return 0;
}