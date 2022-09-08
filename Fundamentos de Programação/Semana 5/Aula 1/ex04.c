#include <stdio.h>

#define X_WEIGHT 2
#define Y_WEIGHT 3
#define Z_WEIGHT 5

int main()
{
  int tests, i;
  float x, y, z, average;

  printf("Digite quantas vezes voce quer testar: ");
  scanf("%d", &tests);

  for (i = 0; i < tests; i++)
  {
    printf("Digite tres valores, separados por espacos: ");
    scanf("%f %f %f", &x, &y, &z);

    average = (x * X_WEIGHT + y * Y_WEIGHT + z * Z_WEIGHT) / (X_WEIGHT + Y_WEIGHT + Z_WEIGHT);

    printf("A media entre os valores digitados eh %.1f\n", average);
  }

  return 0;
}