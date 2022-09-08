#include <stdio.h>

int main()
{
  int num, i, sum = 0;

  printf("Digite um numero inteiro para saber se ele eh perfeito: ");
  scanf("%d", &num);

  for (i = 1; i < num; i++)
  {
    if (num % i == 0)
      sum += i;
  }

  if (sum == num)
    printf("Este numero eh perfeito.\n");
  else
    printf("Este numero nao eh perfeito.\n");

  return 0;
}