#include <stdio.h>

int verificaSegmento(int n1, int n2);
int casasDec(int num);
int primeiroDigito(int num, int casasDecimais);
int removePrimeiroDigito(int num, int casasDecimais);

int main()
{
  int n1, n2;

  printf("Eh segmento - Digite dois numeros: ");
  scanf("%d %d", &n1, &n2);

  printf("%d\n", verificaSegmento(n1, n2));

  return 0;
}

int verificaSegmento(int n1, int n2)
{
  int primeiroDigMenor, maior, menor, casasMenor, aux, aux2, primeiroDigAux, ehSegmento = 0;

  if (n1 == n2)
    return 1;

  maior = n1 > n2 ? n1 : n2;
  menor = n2 < n1 ? n2 : n1;
  casasMenor = casasDec(menor);
  primeiroDigMenor = primeiroDigito(menor, casasMenor);
  aux = maior;

  while (aux > 0 && !ehSegmento)
  {
    aux2 = removePrimeiroDigito(aux, casasDec(aux));

    if (aux2 == 0)
      break;

    primeiroDigAux = primeiroDigito(aux2, casasDec(aux2));

    if (primeiroDigAux == primeiroDigMenor)
      if ((aux2 / menor) % 10 == 0 || aux2 == menor)
        ehSegmento = 1;

    aux = aux2;
  }

  printf("menor: %d\n", menor);

  return ehSegmento ? (menor == n1 ? 1 : 2) : 0;
}

int casasDec(int num)
{
  int casas = 1;

  while (num > 0)
  {
    casas *= 10;
    num /= 10;
  }

  return casas;
}

int primeiroDigito(int num, int casasDecimais)
{
  return num / (casasDecimais / 10);
}

int removePrimeiroDigito(int num, int casasDecimais)
{
  int primeiroDig = primeiroDigito(num, casasDecimais);

  return num - primeiroDig * (casasDecimais / 10);
}