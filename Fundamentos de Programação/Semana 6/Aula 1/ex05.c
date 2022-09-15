#include <stdio.h>

#define CHICO 1.5
#define ZE 1.1

int main()
{
  float chico = CHICO, ze = ZE;
  int years = 0;

  while (ze <= chico)
  {
    ze += 0.03;
    chico += 0.02;
    years++;
  }

  printf("Ze levara %d anos para ser maior que Chico.\n", years);

  return 0;
}