#include <stdio.h>

#define MAX(X, Y) (X >= Y ? X : Y)

int main()
{
  int n1, n2, n3, n4, i, j, k, primoDivisivel, mult = 1;

  printf("Digite dois inteiros: ");
  scanf("%d %d", &n1, &n2);

  n3 = n1;
  n4 = n2;
  k = 0;

  printf("\n");

  while (n1 != 1 || n2 != 1)
  {
    for (i = 2; i <= MAX(n1, n2); i++)
    {
      for (j = 2; j <= i - 1; j++)
        if (i % j == 0)
          continue;

      if (n1 % i == 0 || n2 % i == 0)
      {
        primoDivisivel = i;
        break;
      }
    }

    if (k == 0)
      printf("%d\t%d\t|\t  %d\n", n1, n2, primoDivisivel);
    else
      printf("%d\t%d\t|\tx %d\n", n1, n2, primoDivisivel);

    if (n1 % primoDivisivel == 0)
      n1 /= primoDivisivel;
    if (n2 % primoDivisivel == 0)
      n2 /= primoDivisivel;

    mult = primoDivisivel * mult;
    k++;
  }

  printf("\t\t|----------------\n");
  printf("1\t1\t|\t%d\n\n", mult);
  printf("O MMC de (%d, %d) eh %d", n3, n4, mult);

  return 0;
}