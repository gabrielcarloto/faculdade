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
    printf("O perimetro eh maior que a area.\n");
  }
  else
  {
    printf("O perimetro nao eh maior que a area.\n");
  }

  return 0;
}