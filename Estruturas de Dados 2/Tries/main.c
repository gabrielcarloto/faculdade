#include "trie.h"

int main() {

  Trie *root = create_node('\0');

  root = insert(root, "roupa");
  root = insert(root, "roupas");
  root = insert(root, "rato");
  root = insert(root, "casa");
  root = insert(root, "castor");
  root = insert(root, "mesa");
  root = insert(root, "morro");
  root = insert(root, "gorro");
  root = insert(root, "galho");

  print(root, 0);
  printf("\n\n\n");

  search(root, "mes");
  search(root, "tatu");
  search(root, "galhos");
  search(root, "casa");

  delete (root, "mes");
  delete (root, "tatu");
  delete (root, "galhos");
  delete (root, "roupa");
  delete (root, "casa");
  delete (root, "rato");
  delete (root, "morro");

  root = insert(root, "galinho");
  search(root, "galinho");
  printf("\n\n\n");
  print(root, 0);

  delete (root, "galinho");
  printf("\n\n\n");
  print(root, 0);

  free_node(root);

  return 0;
}