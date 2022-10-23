#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NL 5
#define NC NL
#define NUM_MAX 100 // maximo + 1

#define GREEN "\033[0;32m"
#define DEFAULT "\033[0m"

void geraCartela(int m[][NC], int nl, int nc, int max);
void imprimeMatriz(int m[][NC], int nl, int nc);

int main()
{
  int mat[NL][NC];

  srand(time(NULL));

  geraCartela(mat, NL, NC, NUM_MAX);
  printf("CARTELA GERADA:\n\n");
  imprimeMatriz(mat, NL, NC);
  printf("\n");

  return 0;
}

void geraCartela(int m[][NC], int nl, int nc, int max)
{
  int i, j, k, l, num, numNaoExiste;

  for (i = 0; i < nl; i++)
    for (j = 0; j < nc; j++)
    {
      numNaoExiste = 1;
      num = rand() % max;

      for (k = 0; k <= i; k++)
        for (l = 0; l < nc; l++)
          if (m[k][l] == num)
          {
            numNaoExiste = 0;
            j--;
          }

      if (numNaoExiste)
        m[i][j] = num;
    }
}

void imprimeMatriz(int m[][NC], int nl, int nc)
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