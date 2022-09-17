#include <stdio.h>

int main()
{
  int i, j, mod = 1;

  for (i = 1; mod != 0; i++)
    for (j = 2; j <= 10; j++)
    {
      mod = i % j;

      if (mod != 0)
        break;
    }

  printf("%d", i - 1);

  return 0;
}