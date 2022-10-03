#include <stdio.h>

int calculaDigitoVerificador(int num);

int main()
{
  int num;

  printf("Digite um numero de tres digitos: ");
  scanf("%d", &num);

  printf("%d", calculaDigitoVerificador(num));

  return 0;
}

int calculaDigitoVerificador(int num)
{
  int i, sum = 0;

  for (i = 3; i >= 1; i--)
  {
    sum += (num % 10) * i;
    num /= 10;
  }

  return (sum % 11) % 10;
}