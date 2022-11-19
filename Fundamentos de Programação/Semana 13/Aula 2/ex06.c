#include <stdio.h>
#include <string.h>

#define LIMITE 100

void contaVogais(char str[], int vogais[]);
void imprimeVogais(int contVogais[]);

int main()
{
  int vogais[] = {0, 0, 0, 0, 0};
  char frase[LIMITE];

  printf("Digite uma frase: ");
  fgets(frase, LIMITE, stdin);

  contaVogais(frase, vogais);
  imprimeVogais(vogais);

  return 0;
}

// outra opção seria um struct com as vogais em tipo int

void contaVogais(char str[], int vogais[])
{
  int i;

  for (i = 0; i < strlen(str); i++)
  {
    if (str[i] == 'a' || str[i] == 'A')
      vogais[0]++;
    if (str[i] == 'e' || str[i] == 'E')
      vogais[1]++;
    if (str[i] == 'i' || str[i] == 'I')
      vogais[2]++;
    if (str[i] == 'o' || str[i] == 'O')
      vogais[3]++;
    if (str[i] == 'u' || str[i] == 'U')
      vogais[4]++;
  }
}

void imprimeVogais(int contVogais[])
{
  int i, j;
  char vogais[] = {'a', 'e', 'i', 'o', 'u'};

  for (i = 0; i < 5; i++)
  {
    printf("%c: ", vogais[i]);
    for (j = 0; j < contVogais[i]; j++)
      printf("*");
    printf(" (%d)\n", contVogais[i]);
  }
}