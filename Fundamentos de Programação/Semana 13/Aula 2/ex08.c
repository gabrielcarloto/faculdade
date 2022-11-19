#include <stdio.h>
#include <string.h>
#include "utils.c"

int main()
{
  int i, numPalFrase1, numPalFrase2;
  char frase1[LIM_PADRAO_STR], frase2[LIM_PADRAO_STR], **palavrasFrase1, **palavrasFrase2;

  printf("Digite a primeira frase: ");
  leString(frase1, LIM_PADRAO_STR);

  printf("Digite a segunda frase: ");
  leString(frase2, LIM_PADRAO_STR);

  palavrasFrase1 = split(frase1, " ", &numPalFrase1);
  palavrasFrase2 = split(frase2, " ", &numPalFrase2);

  for (i = 0; i < numPalFrase2; i++)
    printf("%s %s ", palavrasFrase1[i], palavrasFrase2[i]);

  for (i; i < numPalFrase1; i++)
    printf("%s ", palavrasFrase1[i]);

  liberaMatriz(palavrasFrase1, numPalFrase1);
  liberaMatriz(palavrasFrase2, numPalFrase2);

  return 0;
}