#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
  char nome;
  int diaAniversario;
  int mesAniversario;
} Pessoa;

Pessoa *alocaVetor(int tam);
void intToMonth(int month, char *str);

int main()
{
  int tam, i, mes = -1;
  char mesAniversario[10];
  Pessoa *v;

  printf("Digite quantas pessoas vc quer registrar: ");
  scanf("%d", &tam);

  v = alocaVetor(tam);

  printf("Agora, digite a inicial de cada pessoa, o dia e o mes de aniversario.\n");

  for (i = 0; i < tam; i++)
  {
    printf("Pessoa %d: ", i + 1);
    scanf(" %c %d %d", &v[i].nome, &v[i].diaAniversario, &v[i].mesAniversario);
  }

  printf("Aniversarios:\n");

  for (i = 0; i < tam; i++)
  {
    if (mes != v[i].mesAniversario - 1)
    {
      intToMonth(v[i].mesAniversario - 1, mesAniversario);
      printf("  %s\n", mesAniversario);
    }

    printf("    - %c\n", v[i].nome);
    mes = v[i].mesAniversario;
  }

  return 0;
}

Pessoa *alocaVetor(int tam)
{
  Pessoa *v;

  v = (Pessoa *)malloc(tam * sizeof(Pessoa));

  if (v == NULL)
    exit(1);

  return v;
}

void intToMonth(int month, char *str)
{
  if (month == 0)
    strcpy(str, "Janeiro");
  else if (month == 1)
    strcpy(str, "Fevereiro");
  else if (month == 2)
    strcpy(str, "Março");
  else if (month == 3)
    strcpy(str, "Abril");
  else if (month == 4)
    strcpy(str, "Maio");
  else if (month == 5)
    strcpy(str, "Junho");
  else if (month == 6)
    strcpy(str, "Julho");
  else if (month == 7)
    strcpy(str, "Agosto");
  else if (month == 8)
    strcpy(str, "Setembro");
  else if (month == 9)
    strcpy(str, "Outubro");
  else if (month == 10)
    strcpy(str, "Novembro");
  else if (month == 11)
    strcpy(str, "Dezembro");
}