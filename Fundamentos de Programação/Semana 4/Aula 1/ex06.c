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
    return printf("Delta (%.2f) eh negativo, portanto portanto as raizes sao imaginarias.\n", delta);
  }

  raiz1 = (-b + sqrt(delta)) / (2 * a);
  raiz2 = (-b - sqrt(delta)) / (2 * a);

  if (raiz1 == raiz2)
  {
    return printf("Existe apenas uma raiz real (%.2f).\n", raiz1);
  }

  printf("As raizes sao %.2f e %.2f.\n", raiz1, raiz2);

  return 0;
}