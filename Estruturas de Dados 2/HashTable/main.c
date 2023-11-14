#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EMPTY -99999

typedef struct {
  int key;
} Hash;

int main_hash(int k, int M) { return k % M; }

int sec_hash(int k, int M) { return 1 + (k % (M - 1)); }

int linear_probing(int chave, int iter, int M) {
  return (main_hash(chave, M) + iter) % M;
}

int quadratic_probing(int chave, int iter, int M) {
  static const int C1 = 1, C2 = 3;
  return (main_hash(chave, M) + C1 * iter + C2 * iter * iter) % M;
}

int double_hash(int chave, int iter, int M) {
  return (main_hash(chave, M) + iter * sec_hash(chave, M)) % M;
}

/* */
Hash *create_hash(int M) {
  int h;
  Hash *H = (Hash *)malloc(M * sizeof(Hash));
  for (h = 0; h < M; h++) {
    H[h].key = EMPTY;
  }
  return H;
}

/* */
void free_hash(Hash *H) { free(H); }

/* */
int hash_search(Hash *H, int M, int key, int (*hash_fn)(int, int, int)) {
  int j, i = 0;
  do {
    j = hash_fn(key, i, M);
    printf("Testando chave: %d\n", j);
    if (H[j].key == key) {
      return j;
    }
    i++;
  } while ((i != M) || (H[j].key != EMPTY));

  return EMPTY;
}

/* */
int hash_insert(Hash *H, int M, int key, int *colisoes,
                int (*hash_fn)(int, int, int)) {
  int j, i = 0;
  do {
    j = hash_fn(key, i, M);
    if (H[j].key == EMPTY) {
      H[j].key = key;
      return j;
    } else {
      *colisoes = *colisoes + 1;
    }
    i++;
  } while (i != M);
  return EMPTY;
}

/* */
void hash_print(Hash *H, int M) {
  int i;
  for (i = 0; i < M; i++) {
    if (H[i].key != EMPTY) {
      printf("%2d - %d\n", i, H[i].key);
    } else {
      printf("%2d -   \n", i);
    }
  }
}

int main() {
  char *names[3] = {"Sondagem linear", "Sondagem quadrática", "Hash duplo"};
  int (*functions[3])(int, int, int) = {linear_probing, quadratic_probing,
                                        double_hash};

  for (int i = 0; i < 3; i++) {
    printf("\n\n%s\n\n", names[i]);

    int colisoes = 0;
    int M = 11;
    Hash *H = create_hash(M);
    hash_insert(H, M, 10, &colisoes, functions[i]);
    hash_insert(H, M, 22, &colisoes, functions[i]);
    hash_insert(H, M, 31, &colisoes, functions[i]);
    hash_insert(H, M, 4, &colisoes, functions[i]);
    hash_insert(H, M, 15, &colisoes, functions[i]);
    hash_insert(H, M, 28, &colisoes, functions[i]);
    hash_insert(H, M, 17, &colisoes, functions[i]);
    hash_insert(H, M, 88, &colisoes, functions[i]);
    hash_insert(H, M, 59, &colisoes, functions[i]);
    printf("Número de colisões: %d\n", colisoes);
    hash_print(H, M);
    free_hash(H);
  }
  return 0;
}
