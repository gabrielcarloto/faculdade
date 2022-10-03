#include <stdio.h>

int verificaUltimosDigitos(int n1, int n2);

int main()
{
  int n1, n2;

  printf("Ultimos digitos iguais - Digite dois numeros: ");
  scanf("%d %d", &n1, &n2);

  if (verificaUltimosDigitos(n1, n2))
    printf("Sao iguais\n");
  else
    printf("Nao sao iguais\n");

  return 0;
}

int verificaUltimosDigitos(int n1, int n2)
{
  int digitosN1, aux = n2, contDigitosN2 = 1;

  while (aux > 0)
  {
    contDigitosN2 *= 10;
    aux /= 10;
  }

  digitosN1 = n1 % contDigitosN2;

  return digitosN1 == n2 ? 1 : 0;
}