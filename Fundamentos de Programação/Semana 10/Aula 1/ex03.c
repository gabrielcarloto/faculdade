#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TAM 11
#define GREEN "\033[0;32m"
#define DEFAULT "\033[0m"

int main()
{
  int i, vetor[TAM], vetor2[TAM], soma = 0;

  srand(time(NULL));

  printf("Os valores do primeiro vetor sao:\t%s[", GREEN);

  for (i = 0; i < TAM; i++)
  {
    vetor[i] = rand() % 100;
    printf("%d, ", vetor[i]);
  }

  printf("\b\b]%s\nOs valores do segundo vetor sao:\t%s[", DEFAULT, GREEN);

  for (i = 0; i < TAM; i++)
  {
    vetor2[i] = rand() % 100;
    printf("%d, ", vetor2[i]);
  }

  printf("\b\b]%s\n\n", DEFAULT);

  for (i = 0; i < (TAM / 2 + !(TAM % 2 == 0)); i++)
  {
    soma += vetor[i] + vetor2[TAM - i - 1];
    printf("%d + %d\t= %d\n", vetor[i], vetor2[TAM - i - 1], vetor[i] + vetor2[TAM - i - 1]);
  }

  printf("\nO total eh: %d\n", soma);

  return 0;
}