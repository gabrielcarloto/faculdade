#include <stdio.h>
#include <math.h>

int main()
{
  float a, b, c, delta, raiz1, raiz2;

  printf("Digite os valores a b e c, separados por espaco: ");
  scanf("%f %f %f", &a, &b, &c);

  delta = pow(b, 2) - 4 * a * c;

  if (delta < 0)
  {
    return 0;
  }

  raiz1 = (-b + sqrt(delta)) / (2 * a);
  raiz2 = (-b - sqrt(delta)) / (2 * a);

  printf("As raizes sao %.2f e %.2f.\n", raiz1, raiz2);

  return 0;
}