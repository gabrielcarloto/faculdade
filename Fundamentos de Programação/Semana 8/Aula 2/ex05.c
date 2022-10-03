#include <stdio.h>

int retornaMaior(int n1, int n2);

int main()
{
  int n1, n2, n3;

  printf("Digite tres numeros: ");
  scanf("%d %d %d", &n1, &n2, &n3);

  printf("O maior eh %d.\n", retornaMaior(retornaMaior(n1, n2), n3));

  return 0;
}

int retornaMaior(int n1, int n2)
{
  return n1 >= n2 ? n1 : n2;
}