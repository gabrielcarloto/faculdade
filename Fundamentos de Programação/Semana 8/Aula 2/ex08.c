#include <stdio.h>

void determinaValores(int n);

int main()
{
  int num;

  printf("Digite um numero: ");
  scanf("%d", &num);

  determinaValores(num);

  return 0;
}

void determinaValores(int n)
{
  int i, j;

  for (i = 1; i < n; i++)
    for (j = 1; i + j < n; j++)
      printf("%d + %d + %d\n", i, j, n - (i + j));
}