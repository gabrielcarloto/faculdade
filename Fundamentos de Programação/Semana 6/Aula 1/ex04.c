#include <stdio.h>

int main()
{
  int n1, n2, x, y, lcm, mod = 1; // lcm = mmc

  printf("Digite dois numeros para calcular o minimo multiplo comum: ");
  scanf("%d %d", &n1, &n2);

  x = n1,
  y = n2;

  while (mod != 0)
  {
    mod = x % y;
    x = y;
    y = mod;
  }

  lcm = (n1 * n2) / x;

  printf("O MMC eh %d.\n", lcm);

  return 0;
}