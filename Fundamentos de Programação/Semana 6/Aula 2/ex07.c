#include <stdio.h>

int main()
{
  int n, i, calc;

  printf("Digite um numero natural: ");
  scanf("%d", &n);

  for (i = 1; i < n && calc != n; i++)
  {
    calc = i * (i + 1) * (i + 2);

    if (calc == n)
      break;
  }

  if (calc == n)
    printf("Eh triangular. %d x %d x %d = %d.\n", i, i + 1, i + 2, calc);
  else
    printf("Nao eh triangular.\n");

  return 0;
}