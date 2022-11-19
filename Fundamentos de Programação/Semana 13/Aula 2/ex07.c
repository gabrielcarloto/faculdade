#include <stdio.h>
#include <string.h>

#define LIMITE_FRASE 100

void removeEnter(char *str);
int numOcorrencias(char *str, char *word);
int numOcorrenciasAlt(char *str, char *word);

int main()
{
  int ocorrencias;
  char frase[LIMITE_FRASE], palavra[LIMITE_FRASE / 4];

  printf("Digite uma frase: ");
  fgets(frase, LIMITE_FRASE, stdin);
  removeEnter(frase);

  printf("Digite uma palavra: ");
  fgets(palavra, LIMITE_FRASE, stdin);
  removeEnter(palavra);

  // ocorrencias = numOcorrencias(frase, palavra);
  ocorrencias = numOcorrenciasAlt(frase, palavra);

  printf("A palavra \"%s\" ocorre %d vezes em \"%s\".\n", palavra, ocorrencias, frase);

  return 0;
}

int numOcorrencias(char *str, char *word)
{
  int i, j, ehIgual, cont = 0;

  for (i = 0; i < strlen(str); i++)
  {
    if (str[i] == word[0])
    {
      ehIgual = 1;
      for (j = i + 1; j < (strlen(word) + i) && ehIgual; j++)
        if (str[j] != word[j - i])
          ehIgual = 0;

      if (ehIgual)
        cont++;
    }
  }

  return cont;
}

int numOcorrenciasAlt(char *str, char *word)
{
  int cont = 0;
  char *substr = strstr(str, word);

  while (substr)
  {
    cont++;
    substr = strstr(substr + 1, word);
  }

  return cont;
}

void removeEnter(char *str)
{
  int i;

  for (i = 0; i < strlen(str); i++)
    if (str[i] == '\n')
    {
      str[i] = '\0';
      break;
    }
}