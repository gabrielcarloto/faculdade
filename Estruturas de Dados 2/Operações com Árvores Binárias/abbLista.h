#ifndef ABBLISTA_H
#define ABBLISTA_H

#include <stdio.h>
#include <stdlib.h>

typedef struct arvore {
   int info;
   struct arvore *esq;
   struct arvore *dir;
} Arvore;

Arvore* cria_arv_vazia (void);
void arv_libera (Arvore* a);
Arvore* inserir (Arvore *a, int v);
Arvore* remover (Arvore *a, int v);
int buscar (Arvore *a, int v);
void pre_order (Arvore* a); 

#endif
