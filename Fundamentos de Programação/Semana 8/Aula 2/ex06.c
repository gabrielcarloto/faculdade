#include <stdio.h>

int retornaResto(int alexandre, int maria, int rafael);
void imprimeVencedor(int resto);

int main()
{
  int ale, ma, rafa;

  printf("Digite os numeros escolhidos (na ordem alexandre -> maria -> rafael): ");
  scanf("%d %d %d", &ale, &ma, &rafa);

  imprimeVencedor(retornaResto(ale, ma, rafa));

  return 0;
}

int retornaResto(int alexandre, int maria, int rafael)
{
  return (alexandre + maria + rafael) % 3;
}

void imprimeVencedor(int resto)
{
  if (resto == 0)
    printf("Alexandre venceu!\n");
  else if (resto == 1)
    printf("Maria venceu!\n");
  else if (resto == 2)
    printf("Rafael venceu!\n");
}