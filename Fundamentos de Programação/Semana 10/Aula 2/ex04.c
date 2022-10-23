#include <stdio.h>

// Testado até 9x9
#define NL 3
#define NC NL

int main()
{
  int A[NL][NC], num, i, j, somas[NL], ehMagico = 1;

  printf("Verifica quadrado magico - Digite os %d valores inteiros de uma matriz %dx%d.\n", NL * NC, NL, NC);

  for (i = 0; i < NL && ehMagico; i++)
  {
    somas[i] = 0;
    for (j = 0; j < NC; j++)
    {
      scanf("%d", &num);
      A[i][j] = num;
      somas[i] += num;
    }

    if (i > 0 && somas[i] != somas[i - 1])
      ehMagico = 0;
  }

  // Verifica colunas
  for (i = 0; i < NC && ehMagico; i++)
  {
    somas[i] = 0;
    for (j = 0; j < NL; j++)
      somas[i] += A[j][i];

    if (i > 0 && somas[i] != somas[i - 1])
      ehMagico = 0;
  }

  somas[0] = 0;
  somas[1] = 0;

  // Verifica as diagonais
  for (i = 0; i < NL && ehMagico; i++)
  {
    somas[0] += A[i][i];
    somas[1] += A[i][NC - i - 1];
  }

  if (somas[0] != somas[1])
    ehMagico = 0;

  if (ehMagico)
    printf("Eh magico.\n");
  else
    printf("Nao eh magico.\n");

  return 0;
}