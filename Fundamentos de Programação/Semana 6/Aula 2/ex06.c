#include <stdio.h>

int main()
{
  int n, i, num, aux, crescente = 1, contMudancaCresc = 0;

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

    if (crescente && num <= aux || !crescente && num >= aux)
    {
      crescente = 0;
      contMudancaCresc++;
    }

    aux = num;
  }

  if (crescente && contMudancaCresc == 0)
    printf("Lista em ordem crescente.\n");
  else if (!crescente && contMudancaCresc == 1)
    printf("Lista em ordem decrescente.\n");
  else if (!crescente && contMudancaCresc > 1)
    printf("Lista desordenada.\n");

  return 0;
}