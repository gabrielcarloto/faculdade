
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "utils.c"

int main()
{
  int **mat, vizinhas[3], contBrancas, i, j, k, n = 6;

  srand(time(NULL));

  mat = alocaMatriz(n, n);

  for (i = 0; i < n; i++)
    mat[0][i] = rand() % 2;

  for (i = 0; i < n; i++)
    mat[i][0] = rand() % 2;

  for (i = 1; i < n; i++)
  {
    contBrancas = 0;
    for (j = 1; j < n; j++)
    {
      vizinhas[0] = mat[i][j - 1];
      vizinhas[1] = mat[i - 1][j];
      vizinhas[2] = mat[i - 1][j - 1];

      for (k = 0; k < 3; k++)
        contBrancas += vizinhas[k]; // brancas == 1

      mat[i][j] = !(contBrancas > 1);
    }
  }

  imprimeMatriz(mat, n, n);

  return 0;
}