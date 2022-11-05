#include <stdio.h>
#include "utils.c"

int custo_cidades(int *cidades, int n_cidades, int **m);

int main()
{
  int **costs, *route, dim, routeSize, cityCode, routeCost, cityCost, totalCost, i, j;

  printf("Digite a dimensao da matriz de custo: ");
  scanf("%d", &dim);

  costs = alocaMatriz(dim, dim);

  printf("Digite o custo de cada cidade.\n");

  for (i = 0; i < dim; i++)
    for (j = 0; j < dim; j++)
    {
      scanf("%d", &cityCost);
      costs[i][j] = cityCost;
    }

  printf("Digite o tamanho do itinerario: ");
  scanf("%d", &routeSize);

  route = alocaVetor(routeSize);

  printf("Digite o codigo de cada cidade.\n");

  for (i = 0; i < routeSize; i++)
  {
    scanf("%d", &cityCode);

    while (cityCode > dim - 1)
    {
      printf("Voce nao incluiu essa cidade na matriz de custos.\n");
      printf("Os codigos disponiveis sao 0 a %d.\n", dim - 1);
      printf("Por favor, digite outro codigo: ");

      scanf("%d", &cityCode);
    }

    route[i] = cityCode;
  }

  totalCost = custo_cidades(route, routeSize, costs);

  printf("O custo total eh R$ %d,00", totalCost);

  return 0;
}

int custo_cidades(int *cidades, int n_cidades, int **m)
{
  int i, custo = 0;

  for (i = 1; i < n_cidades; i++)
    custo += m[cidades[i - 1]][cidades[i]];

  return custo;
}