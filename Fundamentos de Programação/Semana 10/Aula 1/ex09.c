#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TAM 5
#define GREEN "\033[0;32m"
#define DEFAULT "\033[0m"

void preencheVetor(int *v, int n, int min, int max);
void intercalaVetores(int v1[], int v2[], int *v3, int n);

int main()
{
  int vetor[TAM], vetor2[TAM], vetor3[TAM * 2], i;

  srand(time(NULL));

  preencheVetor(&vetor, TAM, 1, 1000);

  printf("O primeiro vetor gerado eh:\t%s[", GREEN);

  for (i = 0; i < TAM; i++)
    printf("%d, ", vetor[i]);

  printf("\b\b]%s\n", DEFAULT);

  preencheVetor(&vetor2, TAM, 1, 1000);

  printf("O segundo vetor gerado eh:\t%s[", GREEN);

  for (i = 0; i < TAM; i++)
    printf("%d, ", vetor2[i]);

  printf("\b\b]%s\n\n", DEFAULT);

  intercalaVetores(vetor, vetor2, &vetor3, TAM);

  printf("Vetores intercalados:\t\t%s[", GREEN);

  for (i = 0; i < TAM * 2; i++)
    printf("%d, ", vetor3[i]);

  printf("\b\b]%s\n", DEFAULT);

  return 0;
}

void preencheVetor(int *v, int n, int min, int max)
{
  int i;

  for (i = 0; i < n; i++)
    v[i] = min + rand() % (max - min + 1);
}

void intercalaVetores(int v1[], int v2[], int *v3, int n)
{
  int i;

  for (i = 0; i < n * 2; i++)
    v3[i] = i % 2 == 0 ? v1[i / 2] : v2[i / 2];
}