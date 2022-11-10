#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main()
{
  int *counts, size, tests, i;

  srand(time(NULL));

  printf("Teste de Aleatoriedade do rand()\n\nDigite o maior numero randomico a ser gerado: ");
  scanf("%d", &size);
  size += 1;

  counts = (int *)calloc(size, sizeof(int));

  if (counts == NULL)
  {
    printf("Erro ao alocar memoria.\n");
    exit(1);
  }

  printf("Digite o numero de testes: ");
  scanf("%d", &tests);

  for (i = 0; i < tests; i++)
    counts[rand() % size]++;

  for (i = 0; i < size; i++)
    printf("O numero %d foi gerado %d vezes (%.2f%%).\n", i, counts[i], ((float)counts[i] * 100) / tests);

  free(counts);

  return 0;
}