#include <stdio.h>

int ehRegular(int num);

int main()
{
  int num;

  printf("Numero regular - Digite um numero: ");
  scanf("%d", &num);

  printf("%d\n", ehRegular(num));

  return 0;
}

int ehRegular(int num)
{
  int i, fator, regular = 1;

  while (num > 1 && regular)
  {
    fator = 0;

    for (i = 2; i <= num && !fator; i++)
      if (num % i == 0)
        fator = i;

    if (fator > 5)
      regular = 0;

    num /= fator;
  }

  return regular;
}