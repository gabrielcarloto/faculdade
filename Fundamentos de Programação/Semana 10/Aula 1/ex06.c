#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TAM 10
#define GREEN "\033[0;32m"
#define DEFAULT "\033[0m"

void preencheVetor(int *v, int n, int min, int max);

int main()
{
  int vetor[TAM], i;

  srand(time(NULL));

  preencheVetor(&vetor, TAM, 546, 8567);

  printf("O vetor gerado eh:\t%s[", GREEN);

  for (i = 0; i < TAM; i++)
    printf("%d, ", vetor[i]);

  printf("\b\b]%s\n", DEFAULT);

  printf("Vetor invertido:\t%s[", GREEN);

  for (i = TAM - 1; i >= 0; i--)
    printf("%d, ", vetor[i]);

  printf("\b\b]%s\n", DEFAULT);

  return 0;
}

void preencheVetor(int *v, int n, int min, int max)
{
  int i;

  for (i = 0; i < n; i++)
    v[i] = min + rand() % (max - min + 1);
}