#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define DEFAULT "\033[0m"

#define LIM_PADRAO_STR 100

/**
 * VETORES
 */

void preencheVetor(int v[], int tam, int lim)
{
  int i;

  for (i = 0; i < tam; i++)
    v[i] = rand() % (lim + 1);
}

void preencheMatriz(int **m, int nl, int nc, int lim)
{
  int i, j;

  for (i = 0; i < nl; i++)
    for (j = 0; j < nc; j++)
      m[i][j] = rand() % (lim + 1);
}

int *alocaVetor(int tam)
{
  int *v;

  v = (int *)malloc(tam * sizeof(int));

  if (v == NULL)
  {
    fprintf(stderr, "Erro ao alocar vetor.\n");
    exit(1);
  }

  return v;
}

char *alocaVetorChar(int tam)
{
  char *v;

  v = (char *)malloc(tam * sizeof(char));

  if (v == NULL)
  {
    fprintf(stderr, "Erro ao alocar vetor.\n");
    exit(1);
  }

  return v;
}

int *vetorRand(int tam, int lim)
{
  int *v;

  v = alocaVetor(tam);
  preencheVetor(v, tam, lim);

  return v;
}

int **alocaMatriz(int nl, int nc)
{
  int **m, i;

  m = (int **)malloc(nl * sizeof(int *));

  if (m == NULL)
  {
    fprintf(stderr, "Erro ao alocar matriz\n");
    exit(1);
  }

  for (i = 0; i < nl; i++)
  {
    m[i] = (int *)malloc(nl * sizeof(int));

    if (m[i] == NULL)
    {
      fprintf(stderr, "Erro ao alocar matriz.\n");
      exit(1);
    }
  }

  return m;
}

char **alocaMatrizChar(int nl, int nc)
{
  int i;
  char **m;

  m = (char **)malloc(nl * sizeof(char *));

  if (m == NULL)
  {
    fprintf(stderr, "Erro ao alocar matriz.\n");
    exit(1);
  }

  for (i = 0; i < nl; i++)
  {
    m[i] = (char *)malloc(nl * sizeof(char));

    if (m[i] == NULL)
    {

      fprintf(stderr, "Erro ao alocar matriz.\n");
      exit(1);
    }
  }

  return m;
}

void liberaMatriz(void **mat, int nl)
{
  int i;

  for (i = 0; i < nl; i++)
    free(mat[i]);

  free(mat);
}

int *matrizRand(int nl, int nc, int lim)
{
  int **m;

  m = alocaMatriz(nl, nc);
  preencheMatriz(m, nl, nc, lim);

  return m;
}

void imprimeVetor(int v[], int tam)
{
  int i;

  printf("%s[ ", GREEN);

  for (i = 0; i < tam; i++)
    printf("%d, ", v[i]);

  printf("\b\b ]%s\n\n", DEFAULT);
}

void imprimeMatriz(int **m, int nl, int nc)
{
  int i, j;

  printf("%s[", GREEN);

  for (i = 0; i < nl; i++)
  {
    printf("\t");
    for (j = 0; j < nc; j++)
      printf("%d\t", m[i][j]);
    if (i < nl - 1)
      printf("\n");
  }

  printf("]%s", DEFAULT);
}

/**
 * STRINGS
 */

void leString(char *str, int lim)
{
  fgets(str, lim, stdin);
  removeEnter(str);
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

void removeEspacos(char *str)
{
  int i, contSemEspacos = 0;

  for (i = 0; str[i] != '\0'; i++)
    if (str[i] != ' ')
    {
      str[contSemEspacos] = str[i];
      contSemEspacos++;
    }

  str[contSemEspacos] = '\0';
}

char **split(char *str, char *separador, int *contPalavras)
{
  char **palavras, *palavra;
  int i, linhas = LIM_PADRAO_STR;

  palavras = alocaMatrizChar(linhas, LIM_PADRAO_STR);
  palavra = strtok(str, separador);

  for (i = 0; i < linhas && palavra != NULL; i++)
  {
    strcpy(palavras[i], palavra);
    palavra = strtok(NULL, separador);
  }

  if (i < linhas)
    palavras = realloc(palavras, i * sizeof(char **));

  *contPalavras = i;

  return palavras;
}

char *join(char **strs, int numPalavras, char *separador)
{
  char next[LIM_PADRAO_STR], *joinedString;
  int i, chars = numPalavras * strlen(separador) + 1;

  for (i = 0; i < numPalavras; i++)
  {
    chars += strlen(strs[i]);
  }

  joinedString = alocaVetorChar(chars);
  joinedString[chars] = '\0';
  strcpy(joinedString, "");

  for (i = 0; i < numPalavras; i++)
  {
    if (i == numPalavras - 1)
      separador = "";

    snprintf(next, LIM_PADRAO_STR, "%s%s", strs[i], separador);
    strcat(joinedString, next);
  }

  return joinedString;
}