#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TAM 10
#define GREEN "\033[0;32m"
#define DEFAULT "\033[0m"

void preencheVetor(int *v, int n, int min, int max);
int mediaPares(int v[], int n, int *maior, int *menor);

int main()
{
  int vetor[TAM], i, media, maior, menor;

  srand(time(NULL));

  preencheVetor(&vetor, TAM, 1, 1000);

  printf("O vetor gerado eh: %s[", GREEN);

  for (i = 0; i < TAM; i++)
    printf("%d, ", vetor[i]);

  media = mediaPares(vetor, TAM, &maior, &menor);

  printf("\b\b]%s\n", DEFAULT);
  printf("A media entre os valores pares eh: %d\n", media);
  printf("Maior valor: %d\n", maior);
  printf("Menor valor: %d\n", menor);

  return 0;
}

void preencheVetor(int *v, int n, int min, int max)
{
  int i;

  for (i = 0; i < n; i++)
    v[i] = min + rand() % (max - min + 1);
}

int mediaPares(int v[], int n, int *maior, int *menor)
{
  int i, soma = 0;

  *maior = v[0];
  *menor = v[0];

  for (i = 0; i < n; i++)
  {
    soma += v[i];
    *maior = v[i] > *maior ? v[i] : *maior;
    *menor = v[i] < *menor ? v[i] : *menor;
  }

  return soma / n;
}