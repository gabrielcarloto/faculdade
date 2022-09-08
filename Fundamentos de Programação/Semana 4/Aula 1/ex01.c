#include <stdio.h>
#include <math.h>

#define CAPACIDADE_MAX 50

int main()
{
  int alunos, monitores, viagens;

  printf("Insira o número de alunos e o número de monitores separados por um espaço: ");
  scanf("%d %d", &alunos, &monitores);

  if (alunos + monitores > CAPACIDADE_MAX)
  {
    viagens = ceil(((float)alunos + monitores) / CAPACIDADE_MAX);
    printf("Não é possível levar todos em apenas uma viagem. Para levar todos, são necessárias %d viagens.\n", viagens);
  }
  else
  {
    printf("É possível levar todos em uma viagem.\n");
  }

  return 0;
}
