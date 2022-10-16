#include <stdio.h>

void converteHora(int totalSegundos, int *hora, int *min, int *seg);

int main()
{
  int totalSegundos, horas, min, seg;

  printf("Digite o total em segundos: ");
  scanf("%d", &totalSegundos);

  converteHora(totalSegundos, &horas, &min, &seg);

  printf("%dh %dmin %dseg", horas, min, seg);

  return 0;
}

void converteHora(int totalSegundos, int *hora, int *min, int *seg)
{
  *hora = totalSegundos / 60 / 60;
  *min = totalSegundos / 60 % 60;
  *seg = totalSegundos % 60;
}