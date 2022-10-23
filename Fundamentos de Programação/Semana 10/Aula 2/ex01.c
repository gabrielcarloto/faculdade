#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NL 5
#define NC NL
#define GREEN "\033[0;32m"
#define DEFAULT "\033[0m"

void preencheMatriz(int m[][NC], int nl, int nc, int max);

int main()
{
  int mat[NL][NC], i, j;

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

  printf("]%s\n", DEFAULT);

  printf("Matriz transposta:\n\n%s[", GREEN);

  for (i = 0; i < NL; i++)
  {
    printf("\t");
    for (j = 0; j < NC; j++)
      printf("%d\t", mat[j][i]);
    if (i < NL - 1)
      printf("\n");
  }

  printf("]%s\n", DEFAULT);

  return 0;
}

void preencheMatriz(int m[][NC], int nl, int nc, int max)
{
  int i, j;

  for (i = 0; i < nl; i++)
    for (j = 0; j < nc; j++)
      m[i][j] = rand() % max;
}
