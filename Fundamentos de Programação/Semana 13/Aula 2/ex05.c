#include <stdio.h>
#include <stdlib.h>

#define NUM_FUNCIONARIOS 6

typedef struct
{
  int mat;   /* matricula do funcionario */
  char nome; /* inicial do nome do funcionario */
  int sup;   /* matricula do superior imediato */
} Funcionario;

int Indice_funcionario(Funcionario *vet, int n, int mat);
Funcionario *determinaSubordinado(Funcionario **vet, int n, int mat, int *pTam);
Funcionario *alocaVetor(int tam);
void mockarDados(Funcionario vet[]);
void mockarDadosPt(Funcionario **vet);

int main()
{
  /**
   * Parte A
   */
  Funcionario funcionarios[NUM_FUNCIONARIOS];

  mockarDados(funcionarios);

  int teste = Indice_funcionario(funcionarios, NUM_FUNCIONARIOS, 123);

  printf("Funcionario encontrado: %c, mat: %d\n\n", funcionarios[teste].nome, funcionarios[teste].mat);

  /**
   * Parte B
   */
  Funcionario *ptfuncionarios[NUM_FUNCIONARIOS], *subs;
  int ptam, i;

  for (i = 0; i < NUM_FUNCIONARIOS; i++)
    ptfuncionarios[i] = (Funcionario *)malloc(sizeof(Funcionario *));

  mockarDadosPt(ptfuncionarios);

  subs = determinaSubordinado(ptfuncionarios, NUM_FUNCIONARIOS, 126, &ptam);

  printf("Subordinados de %c (matricula %d)\n", ptfuncionarios[3]->nome, ptfuncionarios[3]->mat);
  for (i = 0; i < ptam; i++)
    printf("- %c -> %d\n", subs[i].nome, subs[i].mat);

  return 0;
}

int Indice_funcionario(Funcionario *vet, int n, int mat)
{
  int i;

  for (i = 0; i < n; i++)
    if (vet[i].mat == mat)
      return i;

  return -1;
}

Funcionario *determinaSubordinado(Funcionario **vet, int n, int mat, int *pTam)
{
  Funcionario *subordinados, func;
  int i, contSubordinados = 0;

  subordinados = alocaVetor(n);

  ; // ? por algum motivo não dá erro ao compilar se tiver esse ponto e vírgula
    // ? obs.: não aparece nenhum erro no vscode

  for (i = 0; i < n; i++)
  {
    if (vet[i]->sup == mat)
    {
      subordinados[contSubordinados].mat = vet[i]->mat;
      subordinados[contSubordinados].sup = vet[i]->sup;
      subordinados[contSubordinados].nome = vet[i]->nome;
      contSubordinados++;
    }
  }

  if (contSubordinados < n)
    subordinados = realloc(subordinados, contSubordinados * sizeof(Funcionario));

  *pTam = contSubordinados;

  return subordinados;
}

Funcionario *alocaVetor(int tam)
{
  Funcionario *v;

  v = (Funcionario *)malloc(tam * sizeof(Funcionario));

  if (v == NULL)
  {
    fprintf(stderr, "Erro ao alocar vetor.\n");
    exit(1);
  }

  return v;
}

void mockarDados(Funcionario vet[])
{
  vet[0].nome = 'L';
  vet[0].mat = 123;
  vet[0].sup = 125;

  vet[1].nome = 'D';
  vet[1].mat = 124;
  vet[1].sup = 129;

  vet[2].nome = 'G';
  vet[2].mat = 125;
  vet[2].sup = 126;

  vet[3].nome = 'C';
  vet[3].mat = 126;
  vet[3].sup = -1;

  vet[4].nome = 'B';
  vet[4].mat = 128;
  vet[4].sup = 126;

  vet[5].nome = 'A';
  vet[5].mat = 129;
  vet[5].sup = 128;
}

void mockarDadosPt(Funcionario **vet)
{
  vet[0]->nome = 'L';
  vet[0]->mat = 123;
  vet[0]->sup = 125;

  vet[1]->nome = 'D';
  vet[1]->mat = 124;
  vet[1]->sup = 129;

  vet[2]->nome = 'G';
  vet[2]->mat = 125;
  vet[2]->sup = 126;

  vet[3]->nome = 'C';
  vet[3]->mat = 126;
  vet[3]->sup = -1;

  vet[4]->nome = 'B';
  vet[4]->mat = 128;
  vet[4]->sup = 126;

  vet[5]->nome = 'A';
  vet[5]->mat = 129;
  vet[5]->sup = 128;
}