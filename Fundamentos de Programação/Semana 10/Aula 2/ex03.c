#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NL 10
#define NC NL
#define GREEN "\033[0;32m"
#define DEFAULT "\033[0m"

void preencheMatriz(int m[][NC], int nl, int nc, int max);

int main()
{
  int mat[NL][NC], min, minCol, max, maxLin, maxCol, i, j;

  srand(time(NULL));

  preencheMatriz(mat, NL, NC, 500);
  printf("Matriz gerada:\n\n%s[", GREEN);

  for (i = 0; i < NL; i++)
  {
    printf("\t");
    for (j = 0; j < NC; j++)
      printf("%d\t", mat[i][j]);
    if (i < NL - 1)
      printf("\n");
  }

  printf("]%s\n\n", DEFAULT);

  max = mat[0][0];

  for (i = 0; i < NL; i++)
    for (j = 0; j < NC; j++)
      if (mat[i][j] > max)
      {
        max = mat[i][j];
        maxLin = i;
        maxCol = j;
      }

  min = mat[maxLin][0];

  for (i = 0; i < NC; i++)
    if (mat[maxLin][i] < min)
    {
      min = mat[maxLin][i];
      minCol = i;
    }

  printf("Minimax: %d, na linha %d e coluna %d.\nMaior valor: %d, na coluna %d.\n", min, maxLin + 1, minCol + 1, max, maxCol + 1);

  return 0;
}

void preencheMatriz(int m[][NC], int nl, int nc, int max)
{
  int i, j;

  for (i = 0; i < nl; i++)
    for (j = 0; j < nc; j++)
      m[i][j] = rand() % max;
}