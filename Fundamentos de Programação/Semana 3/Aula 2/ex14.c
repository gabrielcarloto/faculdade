#include <stdio.h>

int main()
{
  int total, cedulas50, cedulas5, cedulas1;

  printf("Digite todo o dinheiro que voce tem no seu banco, por gentileza: ");
  scanf("%d", &total);

  cedulas50 = total / 50;
  cedulas5 = (total % 50) / 5;
  cedulas1 = (total % 50) % 5;

  printf("Voce tem: %d cedulas de 50, %d cedulas de 5 e %d cedulas de 1.\n", cedulas50, cedulas5, cedulas1);

  return 0;
}