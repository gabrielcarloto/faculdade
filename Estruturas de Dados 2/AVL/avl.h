#ifndef _AVL_H
#define _AVL_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>

template <typename T> struct Arvore {
  T chave;
  int altura;
  struct Arvore *esq;
  struct Arvore *dir;
};

int maior(int esq, int dir);

template <typename T> int altura(Arvore<T> *a);

template <typename T> int atualizar_altura(Arvore<T> *a);

template <typename T> int balanceamento(Arvore<T> *a);

template <typename T> Arvore<T> *rotacao_simples_esq(Arvore<T> *a);

template <typename T> Arvore<T> *rotacao_simples_dir(Arvore<T> *a);

template <typename T> Arvore<T> *rotacao_dupla_esq(Arvore<T> *a);

template <typename T> Arvore<T> *rotacao_dupla_dir(Arvore<T> *a);

template <typename T> Arvore<T> *atualizar_fb_dir(Arvore<T> *a);

template <typename T> Arvore<T> *atualizar_fb_esq(Arvore<T> *a);

template <typename T> Arvore<T> *inserir(Arvore<T> *a, T chave);

template <typename T> Arvore<T> *remover(Arvore<T> *a, T chave);

template <typename T> void imprimir_in_order(Arvore<T> *a, int nivel);

template <typename T> Arvore<T> *liberar_arvore(Arvore<T> *a);

/*Função que devolve o maior entre dois números inteiros!*/
int maior(int esq, int dir) { return (esq > dir ? esq : dir); }

/*----------------------*/
template <typename T> int altura(Arvore<T> *a) { return a ? a->altura : -1; }

template <typename T> int atualizar_altura(Arvore<T> *a) {
  return maior(altura(a->esq), altura(a->dir)) + 1;
}

/*----------------------*/
template <typename T> int balanceamento(Arvore<T> *a) {
  return altura(a->dir) - altura(a->esq);
}

/*----------------------*/
template <typename T> Arvore<T> *rotacao_simples_esq(Arvore<T> *a) {
  Arvore<T> *dir = a->dir;

  a->dir = dir->esq;
  dir->esq = a;

  a->altura = atualizar_altura(a);
  dir->altura = atualizar_altura(dir);

  return dir;
}

/*----------------------*/
template <typename T> Arvore<T> *rotacao_simples_dir(Arvore<T> *a) {
  Arvore<T> *esq = a->esq;

  a->esq = esq->dir;
  esq->dir = a;

  a->altura = atualizar_altura(a);
  esq->altura = atualizar_altura(esq);

  return esq;
}

/*----------------------*/
template <typename T> Arvore<T> *rotacao_dupla_esq(Arvore<T> *a) {
  a->dir = rotacao_simples_dir(a->dir);
  return rotacao_simples_esq(a);
}

/*----------------------*/
template <typename T> Arvore<T> *rotacao_dupla_dir(Arvore<T> *a) {
  a->esq = rotacao_simples_esq(a->esq);
  return rotacao_simples_dir(a);
}

/*----------------------*/
template <typename T> Arvore<T> *atualizar_fb_dir(Arvore<T> *a) {
  a->altura = atualizar_altura(a);

  if (balanceamento(a) != 2)
    return a;

  if (balanceamento(a->dir) >= 0)
    return rotacao_simples_esq(a);

  return rotacao_dupla_esq(a);
}

/*----------------------*/
template <typename T> Arvore<T> *atualizar_fb_esq(Arvore<T> *a) {
  a->altura = atualizar_altura(a);

  if (balanceamento(a) != -2)
    return a;

  if (balanceamento(a->esq) <= 0)
    return rotacao_simples_dir(a);

  return rotacao_dupla_dir(a);
}

/*----------------------*/
template <typename T> Arvore<T> *inserir(Arvore<T> *a, T chave) {
  if (a == NULL) {
    a = new Arvore<T>;
    a->esq = a->dir = NULL;
    a->chave = chave;
    a->altura = 0;

    return a;
  }

  if (chave < a->chave) {
    a->esq = inserir(a->esq, chave);
    return atualizar_fb_esq(a);
  }

  a->dir = inserir(a->dir, chave);
  return atualizar_fb_dir(a);
}

/*Função para remover um nó de uma árvore binária de busca balanceada!*/
template <typename T> Arvore<T> *remover(Arvore<T> *a, T chave) {
  if (a == NULL) {
    return NULL;
  } else {
    if (chave < a->chave) {
      a->esq = remover(a->esq, chave);
      a = atualizar_fb_dir(a);
    } else if (chave > a->chave) {
      a->dir = remover(a->dir, chave);
      a = atualizar_fb_esq(a);
    } else {
      if ((a->esq == NULL) && (a->dir == NULL)) {
        delete a;
        a = NULL;
      } else if (a->esq == NULL) {
        Arvore<T> *tmp = a;
        a = a->dir;
        delete tmp;
      } else if (a->dir == NULL) {
        Arvore<T> *tmp = a;
        a = a->esq;
        delete tmp;
      } else {
        Arvore<T> *tmp = a->esq;
        while (tmp->dir != NULL) {
          tmp = tmp->dir;
        }
        a->chave = tmp->chave;
        tmp->chave = chave;
        a->esq = remover(a->esq, chave);
        a = atualizar_fb_dir(a);
      }
    }
    return a;
  }
}

template <typename T> Arvore<T> *liberar_arvore(Arvore<T> *a) {
  if (a == NULL)
    return NULL;

  a->esq = liberar_arvore(a->esq);
  a->dir = liberar_arvore(a->dir);

  delete a;
  return NULL;
}

/*Função para imprimir os nós de uma árvore binária de acordo com um percurso
 * in-ordem!*/
template <typename T> void imprimir_in_order(Arvore<T> *a, int nivel) {
  if (a != NULL) {
    int i;
    for (i = 0; i < nivel; i++) {
      std::cout << "      ";
    }
    // preorder??
    std::cout << "Chave: " << a->chave << " (altura: " << a->altura
              << ", fb: " << balanceamento(a) << ") no nível: " << nivel
              << "\n";
    imprimir_in_order(a->esq, nivel + 1);
    imprimir_in_order(a->dir, nivel + 1);
  }
}

#endif