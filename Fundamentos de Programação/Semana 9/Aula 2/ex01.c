#include <stdio.h>

void questao01(int *a, int *b);

int main()
{
  int a, b;

  printf("Digite dois inteiros: ");
  scanf("%d %d", &a, &b);

  questao01(&a, &b);

  printf("%d %d\n", a, b);

  return 0;
}

void questao01(int *a, int *b)
{
  (*a)--;
  (*b)++;
}