#include <stdio.h>

int main()
{
  int n, aux1, aux2, iguais = 1;

  printf("Digite um numero: ");
  scanf("%d", &n);

  aux1 = n % 10;

  while (n > 0 && iguais)
  {
    aux2 = n % 10;

    if (aux1 != aux2)
      iguais = 0;

    n /= 10;
  }

  if (iguais)
    printf("Todos os digitos sao iguais.\n");
  else
    printf("Os digitos nao sao todos iguais.\n");

  return 0;
}