#include <stdio.h>

int divide(int *m, int *n, int d);
int mmc(int m, int n);

int main()
{
  int m, n;

  printf("Calcula MMC - Digite dois inteiros: ");
  scanf("%d %d", &m, &n);

  printf("Resultado: %d\n", mmc(m, n));

  return 0;
}

int divide(int *m, int *n, int d)
{
  int mEhDiv = *m % d == 0, nEhDiv = *n % d == 0;

  *m = mEhDiv ? *m / d : *m;
  *n = nEhDiv ? *n / d : *n;

  return mEhDiv || nEhDiv;
}

int mmc(int m, int n)
{
  int i, ehDiv, retorno = 1;

  while (m != n)
  {
    ehDiv = 0;

    for (i = 2; i <= (m > n ? m : n) && !ehDiv; i++)
      ehDiv = divide(&m, &n, i);

    retorno *= i - 1;
  }

  return retorno;
}