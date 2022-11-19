#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TAM 15

typedef struct
{
  int cod;
  int tel;
  int preco;
} Loja;

int main()
{
  Loja lojas[TAM];
  int i, precoAux, media = 0;

  srand(time(NULL));

  for (i = 0; i < TAM; i++)
  {
    lojas[i].cod = numAleatorio(11, 99);
    lojas[i].tel = numAleatorio(19000, RAND_MAX) * 913;
    lojas[i].preco = numAleatorio(100, 10000);

    media += lojas[i].preco;
  }

  media /= TAM;

  printf("Numeros das lojas com preco abaixo da media:\n");

  for (i = 0; i < TAM; i++)
  {
    precoAux = lojas[i].preco;

    if (precoAux < media)
    {
      printf("- (%d) 9%d \tR$ %d,00\n", lojas[i].cod, lojas[i].tel, precoAux);
    }
  }

  return 0;
}

int numAleatorio(int min, int max)
{
  return min + rand() % (max - min) + 1;
}