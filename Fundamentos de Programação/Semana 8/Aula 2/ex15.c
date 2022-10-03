#include <stdio.h>

int paridade(int k);

int main()
{
  int n, i, segPar, segParAnt, atePiramidal, contSeg = 0, ehPiramidal = 0;

  printf("Piramidal alternante - Digite quantos numeros quer testar: ");
  scanf("%d", &n);

  for (i = 1; n > 0; i++)
  {
    contSeg++;
    n -= i;
  }

  ehPiramidal = n == 0;
  atePiramidal = -n;

  printf("Digite os numeros (separados por enter).\n");
  segPar = paridade(1);
  segParAnt = segPar;

  printf("segPar: %d, segParAnt: %d\n", segPar, segParAnt);

  for (i = 2; i <= contSeg; i++)
  {
    segPar = paridade(i == contSeg ? contSeg - atePiramidal : i);

    printf("segPar: %d, segParAnt: %d\n", segPar, segParAnt);
    if ((segPar == segParAnt || segPar == -1) && ehPiramidal)
      ehPiramidal = 0;

    segParAnt = segPar;
  }

  if (ehPiramidal)
    printf("Eh piramidal %d-alternante.\n", contSeg);
  else
    printf("Nao eh piramidal alternante.\n");

  return 0;
}

int paridade(int k)
{
  int i, num, antPar, par, retorno = 0;

  scanf("%d", &num);

  par = num % 2;
  antPar = par;

  for (i = 1; i < k; i++)
  {
    if (antPar != par)
      retorno = -1;

    scanf("%d", &num);

    if (retorno)
      continue;

    antPar = par;
    par = num % 2;
  }

  return retorno == -1 ? retorno : par;
}