#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define DEFAULT "\033[0m"

extern int errno;

void preencheVetor(int v[], int tam, int lim)
{
  int i;

  for (i = 0; i < tam; i++)
    v[i] = rand() % (lim + 1);
}

void preencheMatriz(int **m, int nl, int nc, int lim)
{
  int i, j;

  for (i = 0; i < nl; i++)
    for (j = 0; j < nc; j++)
      m[i][j] = rand() % (lim + 1);
}

int *alocaVetor(int tam)
{
  int *v, errnum;

  v = (int *)malloc(tam * sizeof(int));

  if (v == NULL)
  {
    errnum = errno;
    fprintf(stderr, "Erro ao alocar vetor: %s\n", strerror(errnum));
    exit(1);
  }

  return v;
}

int *vetorRand(int tam, int lim)
{
  int *v;

  v = alocaVetor(tam);
  preencheVetor(v, tam, lim);

  return v;
}

int *alocaMatriz(int nl, int nc)
{
  int **m, i, errnum;

  m = (int **)malloc(nl * sizeof(int *));

  if (m == NULL)
  {
    errnum = errno;
    fprintf(stderr, "Erro ao alocar matriz: %s\n", strerror(errnum));
    exit(1);
  }

  for (i = 0; i < nl; i++)
  {
    m[i] = (int *)malloc(nl * sizeof(int));

    if (m[i] == NULL)
    {
      errnum = errno;
      fprintf(stderr, "Erro ao alocar matriz: %s\n", strerror(errnum));
      exit(1);
    }
  }

  return m;
}

int *matrizRand(int nl, int nc, int lim)
{
  int **m;

  m = alocaMatriz(nl, nc);
  preencheMatriz(m, nl, nc, lim);

  return m;
}

void imprimeVetor(int v[], int tam)
{
  int i;

  printf("%s[ ", GREEN);

  for (i = 0; i < tam; i++)
    printf("%d, ", v[i]);

  printf("\b\b ]%s\n\n", DEFAULT);
}

void imprimeMatriz(int **m, int nl, int nc)
{
  int i, j;

  printf("%s[", GREEN);

  for (i = 0; i < nl; i++)
  {
    printf("\t");
    for (j = 0; j < nc; j++)
      printf("%d\t", m[i][j]);
    if (i < nl - 1)
      printf("\n");
  }

  printf("]%s", DEFAULT);
}