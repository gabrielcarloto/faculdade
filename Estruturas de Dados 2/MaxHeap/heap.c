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

void heapify_up(int V[], int size, int i) {
  if (i <= 0 || i >= size)
    return;

  int parent = pai(i);

  if (parent < size && V[parent] < V[i]) {
    trocar(V, parent, i);
    heapify_up(V, size, parent);
  }
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

void heap_decrease_key(int V[], int i, int k, int size) {
  if (i >= size)
    return;

  V[i] = k;
  max_heapify(V, size, i);
}

int main() {
  int V[] = {15, 13, 9, 5, 12, 8, 7, 4, 0, 6, 2, 1};
  int size = sizeof(V) / sizeof(int);

  printf("Vetor inicial: ");
  imprimir(V, size);

  heap_decrease_key(V, 0, 1, size);
  printf("Pós decrease: ");
  imprimir(V, size);

  return 0;
}