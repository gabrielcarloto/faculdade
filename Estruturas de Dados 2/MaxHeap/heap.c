#include <stdio.h>

void swap(int *a, int *b) {
  int temp = *a;
  (*a) = (*b);
  (*b) = temp;
}

int esquerda(int i) { return i * 2 + 1; }

int direita(int i) { return (i + 1) * 2; }

int pai(int i) { return (i - 1) / 2; }

void trocar(int V[], int a, int b) {
  int aux = V[a];
  V[a] = V[b];
  V[b] = aux;
}

void imprimir(int V[], int size) {
  int i;
  for (i = 0; i < size; i++) {
    printf("%d ", V[i]);
  }
  printf("\n");
}

// heapify down
void max_heapify(int V[], int size, int i) {
  int esq = esquerda(i);
  int dir = direita(i);
  int maior = i;

  if (esq < size && V[esq] > V[maior])
    maior = esq;
  if (dir < size && V[dir] > V[maior])
    maior = dir;
  if (maior == i)
    return;

  swap(&V[maior], &V[i]);
  max_heapify(V, size, maior);
}

void build_max_heap(int V[], int size) { /*TERMINAR*/
  for (int i = size / 2 - 1; i >= 0; i--) {
    max_heapify(V, size, i);
  }
}

void heap_sort(int V[], int size) {
  build_max_heap(V, size);
  while (size > 0) {
    size--;
    swap(&V[0], &V[size]);
    max_heapify(V, size, 0);
  }
}

int main() {
  int size = 10;

  int V[] = {4, 1, 3, 2, 16, 9, 10, 14, 8, 7};

  printf("Vetor inicial: ");
  imprimir(V, size);

  heap_sort(V, size);
  printf("Vetor ordenado: ");
  imprimir(V, size);

  return 0;
}