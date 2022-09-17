#include <stdio.h>

int main()
{
  int n, i, num, aux, crescente = 1;

  printf("Digite quantos numeros voce quer testar: ");
  scanf("%d", &n);

  printf("Digite os numeros.\n");

  for (i = 0; i < n; i++)
  {
    scanf("%d", &num);

    if (i == 0)
    {
      aux = num;
      continue;
    }

    if (num <= aux)
      crescente = 0;

    aux = num;
  }

  if (crescente)
    printf("Esta em ordem crescente.\n");
  else
    printf("Nao esta em ordem crescente.\n");

  return 0;
}