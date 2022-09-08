#include <stdio.h>

int main()
{
  int num, i, t1 = 0, t2 = 1, next = t1 + t2;

  printf("Digite quantos termos da sequencia de Fibonacci voce quer ver: ");
  scanf("%d", &num);

  printf("%d %d ", t1, t2);

  for (i = 2; i < num; i++)
  {
    printf("%d ", next);
    t1 = t2;
    t2 = next;
    next = t1 + t2;
  }

  return 0;
}