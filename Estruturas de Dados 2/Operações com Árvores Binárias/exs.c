#include "abbLista.h"
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

Arvore *cria_arvore_vazia(void) { return NULL; }

void arvore_libera(Arvore *a) {
  if (a != NULL) {
    arvore_libera(a->esq);
    arvore_libera(a->dir);
    free(a);
  }
}

//========= Q1 - inserir
Arvore *inserir(Arvore *a, int v) {
  if (a == NULL) {
    a = (Arvore *)malloc(sizeof(Arvore));
    a->esq = NULL;
    a->dir = NULL;
    a->info = v;
  } else if (v < a->info) {
    a->esq = inserir(a->esq, v);
  } else {
    a->dir = inserir(a->dir, v);
  }

  return a;
}

//========= Q1 - remover
Arvore *remover(Arvore *a, int v) {
  if (a == NULL)
    return NULL;

  if (v < a->info) {
    a->esq = remover(a->esq, v);
    return a;
  } else if (v > a->info) {
    a->dir = remover(a->dir, v);
    return a;
  }

  if (a->esq == NULL && a->dir == NULL) {
    free(a);
    return NULL;
  } else if (a->esq == NULL) {
    Arvore *temp = a->dir;
    free(a);
    return temp;
  } else if (a->dir == NULL) {
    Arvore *temp = a->esq;
    free(a);
    return temp;
  }

  Arvore *temp = a->esq;

  while (temp->dir)
    temp = temp->dir;

  a->info = temp->info;
  temp->info = v;

  a->esq = remover(a->esq, v);

  return a;
}

//========= Q1 - busca
int buscar(Arvore *a, int v) {
  if (a == NULL)
    return 0;

  if (v < a->info)
    return buscar(a->esq, v);
  if (v > a->info)
    return buscar(a->dir, v);

  return 1;
}

void busca(Arvore *a, int v) {
  printf("%d existe na arvore? %s\n", v, buscar(a, v) ? "Sim" : "Nao");
}

//========= Q2 - min =====

int min(Arvore *a) {
  if (a == NULL)
    return 0;

  while (a->esq != NULL)
    a = a->esq;

  printf("Valor min: %d\n", a->info);

  return a->info;
}

//========= Q2 - max =====

int max(Arvore *a) {
  if (a == NULL)
    return 0;

  while (a->dir != NULL)
    a = a->dir;

  printf("Valor max: %d\n", a->info);

  return a->info;
}

//========= Q3 - imprime_decrescente =====

void imprime_decrescente(Arvore *a) {
  if (a == NULL)
    return;

  imprime_decrescente(a->dir);
  printf("%d ", a->info);
  imprime_decrescente(a->esq);
}

//========= Q4 - maior ramo =====

int maior_ramo(Arvore *a) {
  if (a == NULL)
    return 0;

  int ramo_esq = maior_ramo(a->esq);
  int ramo_dir = maior_ramo(a->dir);

  return (ramo_esq > ramo_dir ? ramo_esq : ramo_dir) + a->info;
}

void pre_order(Arvore *a) {
  if (a != NULL) {
    printf("%d ", a->info);
    pre_order(a->esq);
    pre_order(a->dir);
  }
}

Arvore *cria_arv_com_n_elementos(int n, bool aleatorio) {
  Arvore *a = cria_arvore_vazia();

  for (int i = 0; aleatorio && i < n; i++) {
    a = inserir(a, rand() % n);
  }

  for (int i = 0; !aleatorio && i < n; i++) {
    a = inserir(a, i);
  }

  return a;
}

void medir_busca(Arvore *a, int n) {
  clock_t start = clock();
  buscar(a, n);
  clock_t end = clock();

  printf("Levou %f s para buscar\n", (float)(end - start) / CLOCKS_PER_SEC);
}

int main() {
  srand(time(NULL));

  // até ex 4
  {
    Arvore *a = cria_arvore_vazia();

    a = inserir(a, 50);
    a = inserir(a, 30);
    a = inserir(a, 90);
    a = inserir(a, 20);
    a = inserir(a, 40);
    a = inserir(a, 95);
    a = inserir(a, 10);
    a = inserir(a, 35);
    a = inserir(a, 45);

    printf("\n");
    pre_order(a);
    printf("\n");

    busca(a, 20);
    busca(a, 10);
    busca(a, 3);
    busca(a, 12);
    busca(a, 95);

    min(a);
    max(a);

    printf("\nOrdem decrescente:\n");
    imprime_decrescente(a);

    printf("\n\nMaior ramo: %d\n\n", maior_ramo(a));

    printf("Deletando valores 10, 30, 90, 50, 3\n");

    remover(a, 10);
    remover(a, 30);
    remover(a, 90);
    remover(a, 50);
    remover(a, 3);

    printf("Preorder: ");
    pre_order(a);
    printf("\n");

    arvore_libera(a);
  }

  // ====== Q5 ====

  printf("Busca em arvore enorme:\n");
  int elementos = 100000;

  {
    printf("Criando ordenada\n");
    Arvore *a = cria_arv_com_n_elementos(elementos, false);
    printf("Criada\n");
    medir_busca(a, elementos);
    arvore_libera(a);
  }

  {
    printf("Criando aleatoria\n");
    Arvore *a = cria_arv_com_n_elementos(elementos, true);
    printf("Criada\n");
    medir_busca(a, elementos);
    arvore_libera(a);
  }

  // A ordenada é mais lenta porque acaba criando uma lista encadeada
  // 1
  //  2
  //   3
  //    4
  //     ...
  //        99999
  // enquanto a aleatória é uma árvore binária de fato, o que a torna mais
  // rápida tanto para criar quanto para buscar

  return 0;
}
