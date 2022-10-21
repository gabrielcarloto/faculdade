#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TAM 10
#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define DEFAULT "\033[0m"

int main()
{
  int i, vetor[TAM], pos, cont = 0;

  srand(time(NULL));

  printf("Os valores no vetor sao: %s[", GREEN);

  for (i = 0; i < TAM; i++)
  {
    vetor[i] = rand() % 10;
    printf("%d, ", vetor[i]);
  }

  printf("\b\b]%s\nDigite qual posicao voce quer remover (ou um numero negativo para parar): ", DEFAULT);
  scanf("%d", &pos);

  if (pos < 0)
    return 0;

  while (pos > TAM)
  {
    printf("%sEste indice nao existe.%s Por favor digite outro: ", RED, DEFAULT);
    scanf("%d", &pos);

    if (pos < 0)
      return 0;
  }

  cont++;

  for (i = 0; i < TAM; i++)
    if (i >= pos)
      vetor[i] = vetor[i + 1];

  while (pos >= 0 && cont != TAM)
  {
    printf("Posicao %d removida. Os valores agora sao: %s[", pos, GREEN);

    for (i = 0; i < TAM - cont; i++)
      printf("%d, ", vetor[i]);

    printf("\b\b]%s\nDigite qual posicao voce quer remover: ", DEFAULT);
    scanf("%d", &pos);

    while (pos > TAM - cont)
    {
      printf("%sEste indice nao existe.%s Por favor digite outro: ", RED, DEFAULT);
      scanf("%d", &pos);
    }

    cont++;

    for (i = 0; i < TAM - cont; i++)
      if (i >= pos)
        vetor[i] = vetor[i + 1];
  }

  if (cont == TAM)
    printf("Voce removeu todos os valores do vetor.\n");

  return 0;
}