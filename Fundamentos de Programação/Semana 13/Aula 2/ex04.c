#include <stdio.h>
#include <math.h>

#define MAX_ALUNOS 15
#define MAX_PROVAS 3

typedef struct
{
  float p[MAX_PROVAS];
  float M;
} Provas;

typedef struct
{
  int ra;
  int frequencia;
  Provas Ps;
} Aluno;

void imprimeResultados(Aluno alunos[], int numAlunos, float medias[], float desvios[]);

int main(void)
{
  int n, i, j;
  float desvios_Ps[MAX_PROVAS] = {0.0, 0.0, 0.0}, medias_Ps[MAX_PROVAS] = {0.0, 0.0, 0.0};
  Aluno vetAlunos[MAX_ALUNOS];

  printf("Digite o numero de alunos (maximo 15): ");
  scanf("%d", &n);

  for (i = 0; i < n; i++)
  {
    printf("Digite o RA e a frequencia do aluno %d: ", i);
    scanf("%d %d", &vetAlunos[i].ra, &vetAlunos[i].frequencia);

    vetAlunos[i].Ps.M = 0;

    for (j = 0; j < MAX_PROVAS; j++)
    {
      printf("Digite a nota da prova %d: ", j);
      scanf("%f", &vetAlunos[i].Ps.p[j]);
      vetAlunos[i].Ps.M += vetAlunos[i].Ps.p[j];
    }

    vetAlunos[i].Ps.M /= MAX_PROVAS;
  }

  for (i = 0; i < n; i++)
    for (j = 0; j < MAX_PROVAS; j++)
      medias_Ps[j] += vetAlunos[i].Ps.p[j];

  for (i = 0; i < MAX_PROVAS; i++)
    medias_Ps[i] /= n;

  for (i = 0; i < n; i++)
    for (j = 0; j < MAX_PROVAS; j++)
      desvios_Ps[j] += pow((vetAlunos[i].Ps.p[j] - medias_Ps[j]), 2);

  for (i = 0; i < MAX_PROVAS; i++)
    desvios_Ps[i] = sqrt(desvios_Ps[i] / MAX_PROVAS);

  printf("\n");

  imprimeResultados(vetAlunos, n, medias_Ps, desvios_Ps);
}

void imprimeResultados(Aluno alunos[], int numAlunos, float medias[], float desvios[])
{
  int i, j;

  for (i = 0; i < 13; i++)
    printf("-");
  printf("\n");

  for (i = 0; i < numAlunos; i++)
  {
    printf("RA: %.9d\n", alunos[i].ra);
    printf("Freq:      %.2d\n", alunos[i].frequencia);

    for (j = 0; j < MAX_PROVAS; j++)
      printf("Prova %d: %.2f\n", j, alunos[i].Ps.p[j]);

    printf("Media:   %.2f\n", alunos[i].Ps.M);

    for (j = 0; j < 13; j++)
      printf("-");

    printf("\n");
  }

  printf("\n");

  for (i = 0; i < MAX_PROVAS; i++)
  {
    printf("Media geral P%d: %.2f\n", i, medias[i]);
    printf("Desvio padrao P%d: %.2f\n\n", i, desvios[i]);
  }
}