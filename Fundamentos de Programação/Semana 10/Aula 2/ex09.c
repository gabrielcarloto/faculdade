#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define ALUNOS 5
#define QUESTOES 10
#define RESULTS_TAM ALUNOS

#define GREEN "\033[0;32m"
#define DEFAULT "\033[0m"

void genMatrix(char m[][QUESTOES], int nl, int nc);
void comparaRes(char gab[], char res[][QUESTOES], int notas[], int questoes, int alunos);

int main()
{
  int results[RESULTS_TAM], randNum, i, j;
  char gab[QUESTOES], res[ALUNOS][QUESTOES];

  srand(time(NULL));

  for (i = 0; i < QUESTOES; i++)
  {
    randNum = rand() % 4 + 1;
    if (randNum == 1)
      gab[i] = 'a';
    else if (randNum == 2)
      gab[i] = 'b';
    else if (randNum == 3)
      gab[i] = 'c';
    else if (randNum == 4)
      gab[i] = 'd';
  }

  genMatrix(res, ALUNOS, QUESTOES);
  comparaRes(gab, res, results, QUESTOES, ALUNOS);

  printf("GABARITO\n\n");

  for (i = 0; i < QUESTOES; i++)
    printf("Questao %d: %c\n", i + 1, gab[i]);

  printf("\nRESPOSTAS\n\n");

  for (i = 0; i < (QUESTOES + 1) * 8 + 1; i++)
    printf("-");
  printf("\n");
  for (i = 0; i < ALUNOS; i++)
  {
    printf("Aluno %d \t", i + 1);
    for (j = 0; j < QUESTOES; j++)
      printf("%c\t", res[i][j]);
    printf("\n");
    for (j = 0; j < (QUESTOES + 1) * 8 + 1; j++)
      printf("-");
    printf("\n");
  }

  printf("\nNOTAS\n\n");

  for (i = 0; i < ALUNOS; i++)
    printf("Aluno %d: %d\n", i + 1, results[i]);

  return 0;
}

void genMatrix(char m[][QUESTOES], int nl, int nc)
{
  int i, j, randNum;

  for (i = 0; i < nl; i++)
    for (j = 0; j < nc; j++)
    {
      randNum = rand() % 4 + 1;

      if (randNum == 1)
        m[i][j] = 'a';
      else if (randNum == 2)
        m[i][j] = 'b';
      else if (randNum == 3)
        m[i][j] = 'c';
      else if (randNum == 4)
        m[i][j] = 'd';
    }
}

void comparaRes(char gab[], char res[][QUESTOES], int notas[], int questoes, int alunos)
{
  int i, j;

  for (i = 0; i < alunos; i++)
  {
    notas[i] = 0;

    for (j = 0; j < questoes; j++)
      if (res[i][j] == gab[j])
        notas[i]++;
  }
}