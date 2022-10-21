#include <stdio.h>

#define TAM 10

int main()
{
  int vetor[TAM], i, j, aux, ehIgual;

  printf("Preencha o vetor de tamanho %d digitando numeros inteiros.\n", TAM);

  for (i = 0; i < TAM; i++)
  {
    scanf("%d", &aux);
    ehIgual = 0;

    for (j = 0; j < TAM && !ehIgual; j++)
      if (aux == vetor[j])
      {
        printf("Sao permitidos apenas valores distintos. Digite outro valor.\n");
        ehIgual = 1;
        i--;
      }

    if (!ehIgual)
      vetor[i] = aux;
  }

  printf("Os valores digitados foram: [");

  for (i = 0; i < TAM; i++)
    printf("%d, ", vetor[i]);

  printf("\b\b]\n");

  return 0;
}