#include <stdio.h>

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
void min_heapify(int V[], int size, int i) {
  int esq = esquerda(i);
  int dir = direita(i);
  int menor = i;

  if (esq < size && V[esq] < V[menor])
    menor = esq;
  if (dir < size && V[dir] < V[menor])
    menor = dir;
  if (menor == i)
    return;

  trocar(V, menor, i);
  min_heapify(V, size, menor);
}

void heapify_up(int V[], int size, int i) {
  if (i <= 0 || i >= size)
    return;

  int parent = pai(i);

  if (parent < size && V[parent] > V[i]) {
    trocar(V, parent, i);
    heapify_up(V, size, parent);
  }
}

void build_min_heap(int V[], int size) {
  for (int i = size / 2 - 1; i >= 0; i--) {
    min_heapify(V, size, i);
  }
}

void heap_decrease_key(int V[], int i, int k, int size) {
  if (i >= size)
    return;

  V[i] = k;
  heapify_up(V, size, i);
}

int heap_extract_min(int V[], int *size) {
  (*size)--;
  int min = V[0];
  trocar(V, 0, *size);
  min_heapify(V, *size, 0);
  return min;
}

int heap_minimum(int V[]) { return V[0]; }

void min_heap_insert(int V[], int *size, int chave) {
  V[*size] = chave;
  (*size)++;
  heapify_up(V, *size, *size - 1);
}

int main() {
  int v[] = {4, 1, 3, 2, 16, 9, 10, 14, 8, 7};
  int size = sizeof(v) / sizeof(int);

  imprimir(v, size);
  build_min_heap(v, size);
  imprimir(v, size);
  heap_extract_min(v, &size);
  imprimir(v, size);
  // Modificando a última chave {16} para o valor {1}!
  heap_decrease_key(v, size - 1, 1, size);
  imprimir(v, size);
  // Inserindo uma nova chave com valor {0}!
  min_heap_insert(v, &size, 0);
  imprimir(v, size);

  return 0;
}