#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NL 10
#define NC NL

#define RED "\033[0;31m"
#define BLUE "\033[34;1m"
#define GRAY "\033[30;1m"
#define GREEN "\033[0;32m"
#define DEFAULT "\033[0m"

void genMatrix(int m[][NC], int nl, int nc, int max);
void printMatrix(int m[][NC], int nl, int nc);
void sumPrintMatrices(int m[][NC], int m2[][NC], int nl, int nc);
void subtractPrintMatrices(int m[][NC], int m2[][NC], int nl, int nc);
void multiplyPrintMatrices(int m[][NC], int num, int nl, int nc);

int main()
{
  int mat[NL][NC], mat2[NL][NC], opcao, num, matrizesCriadas = 0;

  srand(time(NULL));

  printf("%s================================= OPCOES =================================\n", BLUE);
  printf("= %s0 - Criar duas matrizes 10 x 10 com valores inteiros%s                   =\n", GREEN, BLUE);
  printf("= %s1 - Imprimir as matrizes originais%s                                     =\n", RED, BLUE);
  printf("= %s2 - Somar as duas matrizes e imprimir o resultado%s                      =\n", RED, BLUE);
  printf("= %s3 - Subtrair a primeira matriz da segunda e imprimir o resultado%s       =\n", RED, BLUE);
  printf("= %s4 - Multiplicar uma constante a primeira matriz e imprimir o resultado%s =\n", RED, BLUE);
  printf("= %sDigite qualquer outro numero para parar o programa%s                     =\n", GRAY, BLUE);
  printf("==========================================================================%s\n\n", DEFAULT);

  printf("Digite sua opcao: ");
  scanf("%d", &opcao);

  if (opcao == 0)
    matrizesCriadas = 1;

  while (opcao >= 0 && opcao <= 4 && !matrizesCriadas)
  {
    printf("Voce deve criar as matrizes antes de usar esta opcao. Selecione outra: ");
    scanf("%d", &opcao);

    if (opcao == 0)
      matrizesCriadas = 1;
  }

  genMatrix(mat, NL, NC, 100);
  genMatrix(mat2, NL, NC, 100);
  printf("Matrizes criadas!\n\n");

  while (opcao >= 0 && opcao <= 4)
  {
    printf("%s================================= OPCOES =================================\n", BLUE);
    printf("= %s0 - Criar duas matrizes 10 x 10 com valores inteiros%s                   =\n", GREEN, BLUE);
    printf("= %s1 - Imprimir as matrizes originais%s                                     =\n", GREEN, BLUE);
    printf("= %s2 - Somar as duas matrizes e imprimir o resultado%s                      =\n", GREEN, BLUE);
    printf("= %s3 - Subtrair a primeira matriz da segunda e imprimir o resultado%s       =\n", GREEN, BLUE);
    printf("= %s4 - Multiplicar uma constante a primeira matriz e imprimir o resultado%s =\n", GREEN, BLUE);
    printf("= %sDigite qualquer outro numero para parar o programa%s                     =\n", GRAY, BLUE);
    printf("==========================================================================%s\n\n", DEFAULT);

    printf("Digite sua opcao: ");
    scanf("%d", &opcao);

    if (opcao == 0)
    {
      genMatrix(mat, NL, NC, 100);
      genMatrix(mat2, NL, NC, 100);
      printf("Matrizes criadas!\n\n");
    }
    else if (opcao == 1)
    {
      printf("Primeira matriz:\n\n");
      printMatrix(mat, NL, NC);
      printf("Segunda matriz:\n\n");
      printMatrix(mat2, NL, NC);
    }
    else if (opcao == 2)
    {
      printf("A soma das matrizes eh:\n\n");
      sumPrintMatrices(mat, mat2, NL, NC);
    }
    else if (opcao == 3)
    {
      printf("A subtracao das matrizes eh:\n\n");
      subtractPrintMatrices(mat, mat2, NL, NC);
    }
    else if (opcao == 4)
    {
      printf("Digite a constante: ");
      scanf("%d", &num);
      printf("A multiplicacao das matrizes por %d eh:\n\n", num);
      multiplyPrintMatrices(mat, num, NL, NC);
    }
  }

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

void sumPrintMatrices(int m[][NC], int m2[][NC], int nl, int nc)
{
  int i, j, m3[NL][NC];

  for (i = 0; i < nl; i++)
    for (j = 0; j < nc; j++)
      m3[i][j] = m[i][j] + m2[i][j];

  printMatrix(m3, NL, NC);
}

void subtractPrintMatrices(int m[][NC], int m2[][NC], int nl, int nc)
{
  int i, j, m3[NL][NC];

  for (i = 0; i < nl; i++)
    for (j = 0; j < nc; j++)
      m3[i][j] = m[i][j] - m2[i][j];

  printMatrix(m3, NL, NC);
}

void multiplyPrintMatrices(int m[][NC], int num, int nl, int nc)
{
  int i, j, m3[NL][NC];

  for (i = 0; i < nl; i++)
    for (j = 0; j < nc; j++)
      m3[i][j] = m[i][j] * num;

  printMatrix(m3, NL, NC);
}