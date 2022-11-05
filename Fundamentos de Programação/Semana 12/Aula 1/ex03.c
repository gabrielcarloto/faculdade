#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "utils.c"

#define MIN 5
#define MAX 20

int vetorSemRepeticoes(int v1[], int **v2, int tam);

int main()
{
  int *v1, *v2, v1Len, v2Len;

  srand(time(NULL));

  v1Len = (MIN + (rand() % (MAX - MIN + 1)));
  v1 = vetorRand(v1Len, 10);

  printf("Vetor criado:\n\n");
  imprimeVetor(v1, v1Len);

  v2Len = vetorSemRepeticoes(v1, &v2, v1Len);

  printf("Vetor sem repeticoes:\n\n");
  imprimeVetor(v2, v2Len);

  free(v1);
  free(v2);

  return 0;
}

int vetorSemRepeticoes(int v1[], int **v2, int tam)
{
  int i, j, *vAux, numExiste, v2Len = 0, indShift = 0;

  vAux = (int *)malloc(tam * sizeof(int));

  if (vAux == NULL)
  {
    printf("Deu ruim com o vAux\n");
    exit(1);
  }

  for (i = 0; i < tam; i++)
  {
    numExiste = 0;

    for (j = 0; j < i && !numExiste; j++)
      if (v1[i] == vAux[j])
        numExiste = 1;

    if (numExiste)
      indShift++;

    else
    {
      vAux[i - indShift] = v1[i];
      v2Len++;
    }
  }

  *v2 = (int *)realloc(vAux, tam * sizeof(int));

  return v2Len;
}