#include <stdio.h>

int main()
{
  const float precoGasolina = 5.60, precoPedagio = 8.00, eficiencia = 15.00;
  int distancia, pedagios;
  float custo;

  printf("Digite a distancia e o numero de pedagios da sua viagem, separados por virgula: ");
  scanf("%d,%d", &distancia, &pedagios);

  custo = ((distancia / eficiencia) * precoGasolina) + (pedagios * precoPedagio);

  printf("O custo total da viagem eh R$ %.2f\n", custo);

  return 0;
}