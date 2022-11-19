#include <stdio.h>
#include "utils.c"

typedef struct
{
  int horas;
  int minutos;
  int segundos;
} Tempo;

Tempo segParaTempo(int sec);

int main()
{
  int sec;
  Tempo tmp;

  printf("Digite o total de segundos: ");
  scanf("%d", &sec);

  tmp = segParaTempo(sec);

  printf("%.2dh %.2dm %.2ds\n", tmp.horas, tmp.minutos, tmp.segundos);

  return 0;
}

Tempo segParaTempo(int sec)
{
  Tempo tmp;

  tmp.horas = sec / 60 / 60 % 60;
  tmp.minutos = sec / 60 % 60;
  tmp.segundos = sec % 60;

  return tmp;
}