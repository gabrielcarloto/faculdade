#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TAM 10
#define GREEN "\033[0;32m"
#define DEFAULT "\033[0m"

void preencheVetor(int v[], int n, int min, int max);
int mediaPares(int v[], int n);

int main()
{
  int vetor[TAM], i;

  srand(time(NULL));

  preencheVetor(&vetor, TAM, 1, 1000);

  printf("O vetor gerado eh: %s[", GREEN);

  for (i = 0; i < TAM; i++)
    printf("%d, ", vetor[i]);

  printf("\b\b]%s\nA media entre os valores pares eh: %d\n", DEFAULT, mediaPares(vetor, TAM));

  return 0;
}

void preencheVetor(int v[], int n, int min, int max)
{
  int i;

  for (i = 0; i < n; i++)
    v[i] = min + rand() % (max - min + 1);
}

int mediaPares(int v[], int n)
{
  int i, soma = 0;

  for (i = 0; i < n; i++)
    soma += v[i];

  return soma / n;
}