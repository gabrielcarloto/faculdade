#include <stdio.h>

int main()

{
  int num, i, j, k;

  printf("Digite um inteiro positivo: ");
  scanf("%d", &num);

  printf("\n");

  for (i = 1; i <= num * 16 + 9; i++)
    printf("-");

  printf("\n\t|\t");

  for (i = 1; i <= num; i++)
    printf("%d\t|\t", i);

  printf("\n");

  for (i = 1; i <= num * 16 + 9; i++)
    printf("-");

  printf("\n");

  for (i = 1; i <= num; i++)
  {
    printf("%d\t|\t", i);

    for (j = 1; j <= i; j++)
      printf("%d\t|\t", i * j);

    printf("\b");

    for (k = j; k <= num; k++)
      printf("\t\t|");

    printf("\n");

    for (k = 1; k <= num * 16 + 9; k++)
      printf("-");

    printf("\n");
  }

  return 0;
}