#include <stdio.h>

int main()
{
  int totalSegundos, dias, horas, minutos, segundos;

  printf("Digite o total de segundos: ");
  scanf("%d", &totalSegundos);

  dias = totalSegundos / 60 / 60 / 24;
  horas = totalSegundos / 60 / 60 % 24;
  minutos = totalSegundos / 60 % 60;
  segundos = totalSegundos % 60;

  printf("%d dia(s), %d hora(s), %d minuto(s) e %d segundo(s).\n", dias, horas, minutos, segundos);

  return 0;
}