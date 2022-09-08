#include <stdio.h>

// começa a dar resultados inconsistentes a partir do num 13

int main()
{
  int num, fatorial, i;

  printf("Digite um numero para calcular seu fatorial: ");
  scanf("%d", &num);

  fatorial = num;

  for (i = 1; i < num; i++)
  {
    fatorial = fatorial * (num - i);
  }

  printf("%d", fatorial);

  return 0;
}