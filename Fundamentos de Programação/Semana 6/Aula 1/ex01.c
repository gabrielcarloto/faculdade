#include <stdio.h>

int main()
{
  int userNum = 0, maxNum = 0;

  printf("Digite numeros positivos para saber o maior no final.\n");
  printf("Quando quiser parar, digite um numero negativo.\n");

  while (userNum >= 0)
  {
    scanf("%d", &userNum);
    maxNum = (maxNum + userNum + abs(maxNum - userNum)) / 2;
  }

  printf("O maior numero digitado foi %d.\n", maxNum);

  return 0;
}