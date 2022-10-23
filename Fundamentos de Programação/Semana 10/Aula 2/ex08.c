#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define NL 3
#define NC NL

#define GREEN "\033[0;32m"
#define DEFAULT "\033[0m"

void genMatrix(int m[][NC], int nl, int nc, int max);
void printMatrix(int m[][NC], int nl, int nc);

int main()
{
  int A[NL][NC], B[NL][NC], C[NL][NC], num, i, j;

  srand(time(NULL));

  genMatrix(A, NL, NC, 100);
  genMatrix(B, NL, NC, 100);

  printf("Matriz A:\n\n");
  printMatrix(A, NL, NC);
  printf("Matriz B:\n\n");
  printMatrix(B, NL, NC);

  for (i = 0; i < NL; i++)
    for (j = 0; j < NC; j++)
      C[i][j] = A[i][j] * B[i][j];

  printf("A * B:\n\n");
  printMatrix(C, NL, NC);

  for (i = 0; i < NL; i++)
    for (j = 0; j < NC; j++)
      C[i][j] = A[i][j] * A[i][j];

  printf("A^2:\n\n");
  printMatrix(C, NL, NC);

  printf("Digite um inteiro: ");
  scanf("%d", &num);

  for (i = 0; i < NL; i++)
    for (j = 0; j < NC; j++)
      C[i][j] = pow(A[i][j], num);

  printf("A^%d:\n\n", num);
  printMatrix(C, NL, NC);

  return 0;
}

void genMatrix(int m[][NC], int nl, int nc, int max)
{
  int i, j;

  for (i = 0; i < nl; i++)
    for (j = 0; j < nc; j++)
      m[i][j] = rand() % max;
}

void printMatrix(int m[][NC], int nl, int nc)
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

  printf("]%s\n\n", DEFAULT);
}