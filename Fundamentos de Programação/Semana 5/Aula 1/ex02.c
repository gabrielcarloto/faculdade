#include <stdio.h>
#include <stdbool.h>

int main()
{
  int num, i;
  bool atLeastOneIsEqualToTwo;

  printf("Digite um inteiro e veja os numeros entre 1 e 100 que, divididos por ele, resta 2: ");
  scanf("%d", &num);

  for (i = 0; i < 100; i++)
  {
    if (i % num == 2)
    {
      printf("%d\n", i);
      atLeastOneIsEqualToTwo = true;
    }
  }

  if (!atLeastOneIsEqualToTwo)
  {
    printf("Nenhum numero satisfaz a condicao.\n");
  }

  return 0;
}