#include <stdio.h>

int main()
{
  int x, y, i, count = 0;

  printf("Digite dois numeros e veja quantos numeros impares existem entre eles: ");
  scanf("%d %d", &x, &y);

  for (i = y + 1; i < x; i++)
  {
    if (i % 2 == 1)
      count++;
  }

  printf("Existem %d numeros impares entre %d e %d.\n", count, x, y);

  return 0;
}