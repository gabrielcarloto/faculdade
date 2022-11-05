#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "utils.c"

int *expandeVetor(int *v, int tam, int n, int lim);

int main()
{
  int *vetor, tam, lim, novoTam;

  srand(time(NULL));

  printf("Digite o tamanho do vetor a ser gerado e o limite de cada elemento: ");
  scanf("%d %d", &tam, &lim);

  vetor = vetorRand(tam, lim);
  printf("Vetor gerado:\n\n");
  imprimeVetor(vetor, tam);

  printf("Digite o tamanho a ser adicionado ao mesmo vetor: ");
  scanf("%d", &novoTam);

  vetor = expandeVetor(vetor, tam, novoTam, lim);
  printf("Vetor expandido:\n\n");
  imprimeVetor(vetor, tam + novoTam);

  free(vetor);

  return 0;
}

int *expandeVetor(int *v, int tam, int n, int lim)
{
  int *vetorTemp, i;

  vetorTemp = (int *)realloc(v, (tam + n) * sizeof(int));

  if (vetorTemp == NULL)
  {
    return v;
  }

  for (i = tam; i < tam + n; i++)
    vetorTemp[i] = rand() % (lim + 1);

  return vetorTemp;
}