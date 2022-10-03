#include <stdio.h>

int contadigitos(int n, int d);
int ehPermutacao(int n1, int n2);

int main()
{
  int n1, n2;

  printf("Permutacao - Digite dois numeros: ");
  scanf("%d %d", &n1, &n2);

  if (ehPermutacao(n1, n2))
    printf("Eh permutacao\n");
  else
    printf("Nao eh permutacao\n");

  return 0;
}

int contadigitos(int n, int d)
{
  int cont = 0;

  while (n > 0)
  {
    if (n % 10 == d)
      cont++;

    n /= 10;
  }

  return cont;
}

int ehPermutacao(int n1, int n2)
{
  int mod, ehPermut = 1, aux = n2;

  while (aux > 0 && ehPermut)
  {
    mod = aux % 10;

    if (contadigitos(n2, mod) != contadigitos(n1, mod))
      ehPermut = 0;

    aux /= 10;
  }

  return ehPermut;
}