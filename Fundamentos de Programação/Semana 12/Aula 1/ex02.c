#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "utils.c"

int main()
{
  int *v1, *v2, *vMult, v1Len, v2Len, i, j;

  srand(time(NULL));

  printf("Multiplica vetores - Digite o numero de indices dos dois vetores a serem gerados: ");
  scanf("%d %d", &v1Len, &v2Len);

  v1 = vetorRand(v1Len, 10);
  v2 = vetorRand(v2Len, 10);

  printf("Vetores criados:\n\n");
  imprimeVetor(v1, v1Len);
  imprimeVetor(v2, v2Len);

  vMult = (int *)malloc(v1Len * v2Len * sizeof(int));

  for (i = 0; i < v1Len; i++)
    for (j = 0; j < v2Len; j++)
      vMult[j + i * v2Len] = v1[i] * v2[j];

  printf("Vetores multiplicados:\n\n");
  imprimeVetor(vMult, v1Len * v2Len);

  free(v1);
  free(v2);
  free(vMult);

  return 0;
}