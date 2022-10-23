#include <stdio.h>

#define NL 3
#define NC NL

int main()
{
  int A[NL][NC], num, i, j, k, posNaoNulas[NL], ehPerm = 1;

  printf("Verifica matriz de permutacao - Digite os %d valores binarios de uma matriz %dx%d.\n", NL * NC, NL, NC);

  for (i = 0; i < NL; i++)
    for (j = 0; j < NC && ehPerm; j++)
    {
      scanf("%d", &num);
      A[i][j] = num;

      if (num == 1)
      {
        if (j > 0)
          for (k = 0; k < j; k++)
            if (A[i][k] == A[i][j])
              ehPerm = 0;

        if (i > 0)
          for (k = 0; k < i; k++)
            if (A[k][j] == A[i][j])
              ehPerm = 0;
      }
    }

  if (ehPerm)
    printf("Eh matriz de permutacao.\n");
  else
    printf("Nao eh matriz de permutacao.\n");

  return 0;
}