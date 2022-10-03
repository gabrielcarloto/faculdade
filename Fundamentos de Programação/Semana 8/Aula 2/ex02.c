#include <stdio.h>

int arredonda(float x);

int main()
{
  float num;

  printf("Digite um numero: ");
  scanf("%f", &num);

  printf("%d", arredonda(num));

  return 0;
}

int arredonda(float x)
{
  return x >= 0 ? (int)(x + 0.5) : (int)(x - 0.5);
}