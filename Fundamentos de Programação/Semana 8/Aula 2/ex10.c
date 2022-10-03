#include <stdio.h>

int retornaMDC(int num1, int num2);

int main()
{
  int n1, n2;

  printf("Calculadora de MDC - Digite dois numeros: ");
  scanf("%d %d", &n1, &n2);

  printf("%d\n", retornaMDC(n1, n2));

  return 0;
}

int retornaMDC(int num1, int num2)
{
  while (num1 != num2)
  {
    if (num1 > num2)
      num1 = num1 - num2;
    else if (num2 > num1)
      num2 = num2 - num1;
  }

  return num1;
}