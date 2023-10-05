#include "trie.h"

int main() {
  // lista
  {
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

    printf("Lista\n\n\n");
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
  }

  // revisão para a prova
  {
    Trie *root = create_node('\0');

    root = insert(root, "ar");
    root = insert(root, "ara");
    root = insert(root, "arar");
    root = insert(root, "arara");
    root = insert(root, "aras");
    root = insert(root, "arrasa");
    root = insert(root, "arrasar");
    root = insert(root, "arrasara");
    root = insert(root, "as");

    root = delete(root, "arara");
    root = delete(root, "ara");
    
    root = insert(root, "asa");
    root = insert(root, "asas");
    root = insert(root, "assa");
    root = insert(root, "assar");
    root = insert(root, "assara");
    root = insert(root, "ra");
    root = insert(root, "rara");
    root = insert(root, "raras");

    root = delete(root, "asas");
    root = delete(root, "assa");

    root = insert(root, "ras");
    root = insert(root, "rasa");
    root = insert(root, "rasas");
    root = insert(root, "sa");
    root = insert(root, "saara");
    root = insert(root, "sara");
    root = insert(root, "sarar");
    root = insert(root, "sararas");
    root = insert(root, "saras");

    root = delete(root, "rasas");
    root = delete(root, "sara");

    printf("\n\n\nRevisao\n\n\n");
    print(root, 0);
    
    free_node(root);
  }

  // teste
  {
    Trie *root = create_node('\0');

    root = insert(root, "asa");

    printf("\n\n\nTESTE\n\n\n");
    print(root, 0);

    root = delete(root, "asa");

    printf("\n\n\n\"asa\" removida, nao eh suposto imprimir nada depois disso\n\n\n");
    print(root, 0);
  }

  return 0;
}