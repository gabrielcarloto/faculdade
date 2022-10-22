#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TAM 10
#define GREEN "\033[0;32m"
#define DEFAULT "\033[0m"

void preencheVetor(int v[], int n, int lim);

int main()
{
  int vetor[TAM], i;

  srand(time(NULL));

  preencheVetor(&vetor, TAM, 100);

  printf("O vetor gerado eh: %s[", GREEN);

  for (i = 0; i < TAM; i++)
    printf("%d, ", vetor[i]);

  printf("\b\b]%s\n", DEFAULT);

  return 0;
}

void preencheVetor(int v[], int n, int lim)
{
  int i;

  for (i = 0; i < n; i++)
    v[i] = rand() % lim;
}