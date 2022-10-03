#include <stdio.h>
#include <math.h>

int bin2dec(int num);
int dec2bin(int num);

int main()
{
  int num, op;

  printf("========= OPCOES =========\n");
  printf("= 0 = Parar o programa   =\n");
  printf("= 1 = Binario -> decimal =\n");
  printf("= 2 = Decimal -> binario =\n");
  printf("==========================\n\n");

  printf("Digite sua opcao: ");
  scanf("%d", &op);

  while (op != 0)
  {
    printf("Digite o numero: ");
    scanf("%d", &num);

    if (op == 1)
      printf("%d\n", bin2dec(num));
    else if (op == 2)
      printf("%d\n", dec2bin(num));

    printf("Digite sua opcao: ");
    scanf("%d", &op);
  }

  return 0;
}

int bin2dec(int num)
{
  int i, aux = num, dec = 0, numOfDigits = 0;

  while (aux > 0)
  {
    numOfDigits++;
    aux /= 10;
  }

  for (i = 0; i < numOfDigits; i++)
  {
    dec += (num % 10) * pow(2, i);
    num /= 10;
  }

  return dec;
}

int dec2bin(int num)
{
  int rem, bin = 0, digit = 1;

  while (num > 0)
  {
    rem = num % 2;
    num /= 2;
    bin += rem * digit;
    digit *= 10;
  }

  return bin;
}