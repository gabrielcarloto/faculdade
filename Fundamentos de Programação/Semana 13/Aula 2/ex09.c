#include <stdio.h>
#include "utils.c"

int verificaVogal(char letra);

int main()
{
  char nome[LIM_PADRAO_STR], bairro[LIM_PADRAO_STR], senha[LIM_PADRAO_STR] = "";
  int i;

  printf("Digite seu nome: ");
  leString(nome, LIM_PADRAO_STR);
  printf("Digite seu bairro: ");
  leString(bairro, LIM_PADRAO_STR);

  removeEspacos(nome);
  removeEspacos(bairro);

  for (i = 0; nome[i] != '\0'; i++)
    if (verificaVogal(nome[i]))
      strncat(senha, &nome[i], 1);

  for (i = 0; bairro[i] != '\0'; i++)
    if (verificaVogal(bairro[i]))
      strncat(senha, &bairro[i], 1);

  printf("Sua senha eh %s\n", senha);

  return 0;
}

int verificaVogal(char letra)
{
  return (
      letra == 'a' ||
      letra == 'e' ||
      letra == 'i' ||
      letra == 'o' ||
      letra == 'u' ||
      letra == 'A' ||
      letra == 'E' ||
      letra == 'I' ||
      letra == 'O' ||
      letra == 'U');
}