#include <stdio.h>

int calcula_raizes(float a, float b, float c, float *x1, float *x2);

int main()
{
  float a, b, c, x1, x2;
  int numRaizes;

  printf("Digite os valores a, b e c da equacao do segundo grau: ");
  scanf("%f %f %f", &a, &b, &c);

  numRaizes = calcula_raizes(a, b, c, &x1, &x2);

  if (numRaizes == 2)
    printf("As raizes sao %.2f e %.2f.\n", x1, x2);
  else if (numRaizes == 1)
    printf("Existe apenas uma raiz real: %.2f.\n", x1);
  else
    printf("Nao existem raizes reais.\n");

  return 0;
}

int calcula_raizes(float a, float b, float c, float *x1, float *x2)
{
  float delta = sqrt(b * b - 4 * a * c);

  *x1 = (-b + delta) / 2 * a;
  *x2 = (-b - delta) / 2 * a;

  return delta > 0 ? 2 : (delta == 0 ? 1 : 0);
}