#include <stdio.h>
#include "utils.c"

int main()
{
  char nome[LIM_PADRAO_STR], **nomes;
  int contNomes, i;

  printf("Digite seu nome completo: ");
  leString(nome, LIM_PADRAO_STR);

  nomes = split(nome, " ", &contNomes);

  printf("Abreviatura: ");

  for (i = 0; i < contNomes; i++)
    if (strlen(nomes[i]) - 1 > 2)
      printf("%c. ", nomes[i][0]);
    else
      printf("%s ", nomes[i]);

  liberaMatriz(nomes, contNomes);

  return 0;
}