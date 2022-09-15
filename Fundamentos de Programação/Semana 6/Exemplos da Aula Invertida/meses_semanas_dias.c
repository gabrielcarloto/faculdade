#include <stdio.h>

int main()
{
  int i, j, k, count = 0;

  for (i = 1; i <= 12; i++)
  {
    printf("Mes %d\n", i);

    for (j = 1; j <= 4; j++)
    {
      printf("|  Semana %d\n", j);

      for (k = 1; k <= 7; k++)
      {
        count++;
        printf("|  |  Dia %d (%d)\n", k, count);
      }
    }

    printf("\n");
  }

  return 0;
}