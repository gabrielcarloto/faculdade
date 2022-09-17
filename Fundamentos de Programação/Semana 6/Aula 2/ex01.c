#include <stdio.h>

int main()
{
  int n, m, i, j;

  printf("Digite dois inteiros positivos: ");
  scanf("%d %d", &n, &m);

  for (i = 1; i <= n; i++)
  {
    for (j = 1; j <= m; j++)
      printf("%d ", i * j);

    printf("\n");
  }

  return 0;
}