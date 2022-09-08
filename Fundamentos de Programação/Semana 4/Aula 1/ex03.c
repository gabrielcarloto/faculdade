#include <stdio.h>

int main()
{
  int base, altura, perimetro, area;

  printf("Digite a base e a altura do retangulo, separados por espacos: ");
  scanf("%d %d", &base, &altura);

  perimetro = base * 2 + altura * 2;
  area = base * altura;

  if (perimetro > area)
  {
    printf("O perimetro (%d) eh maior que a area (%d).\n", perimetro, area);
  }
  else if (area > perimetro)
  {
    printf("A area (%d) eh maior que o perimetro (%d).\n", area, perimetro);
  }

  return 0;
}