#include "avl.h"
#include <string>
int main() {
  std::cout << "Inicio do programa \n\n\n";
  {
    printf("Exercicio 1\n\n");
    Arvore<char> *AVL = NULL;

    AVL = inserir(AVL, 'Q');
    AVL = inserir(AVL, 'Z');
    AVL = inserir(AVL, 'B');
    AVL = inserir(AVL, 'Y');
    AVL = inserir(AVL, 'T');
    AVL = inserir(AVL, 'M');
    AVL = inserir(AVL, 'E');
    AVL = inserir(AVL, 'W');
    AVL = inserir(AVL, 'X');
    AVL = inserir(AVL, 'S');
    AVL = inserir(AVL, 'F');
    AVL = inserir(AVL, 'G');
    AVL = inserir(AVL, 'A');
    AVL = inserir(AVL, 'H');
    AVL = inserir(AVL, 'N');
    AVL = inserir(AVL, 'O');
    AVL = inserir(AVL, 'P');
    AVL = inserir(AVL, 'R');

    imprimir_in_order(AVL, 0);

    AVL = remover(AVL, 'A');
    AVL = remover(AVL, 'H');
    AVL = remover(AVL, 'E');
    AVL = remover(AVL, 'W');
    AVL = remover(AVL, 'G');
    AVL = remover(AVL, 'N');
    AVL = remover(AVL, 'P');
    AVL = remover(AVL, 'O');

    printf("\n\nApos remover itens\n\n");

    imprimir_in_order(AVL, 0);
    liberar_arvore(AVL);
  }

  {
    printf("\n\nExercicio 2\n\n");

    Arvore<int> *AVL = nullptr;

    AVL = inserir(AVL, 14);
    AVL = inserir(AVL, 5);
    AVL = inserir(AVL, 28);
    AVL = inserir(AVL, 2);
    AVL = inserir(AVL, 8);
    AVL = inserir(AVL, 18);
    AVL = inserir(AVL, 33);
    AVL = inserir(AVL, 1);
    AVL = inserir(AVL, 3);
    AVL = inserir(AVL, 6);
    AVL = inserir(AVL, 11);
    AVL = inserir(AVL, 15);
    AVL = inserir(AVL, 20);
    AVL = inserir(AVL, 30);
    AVL = inserir(AVL, 38);
    AVL = inserir(AVL, 4);
    AVL = inserir(AVL, 7);
    AVL = inserir(AVL, 10);
    AVL = inserir(AVL, 12);
    AVL = inserir(AVL, 16);
    AVL = inserir(AVL, 19);
    AVL = inserir(AVL, 21);
    AVL = inserir(AVL, 29);
    AVL = inserir(AVL, 31);
    AVL = inserir(AVL, 35);
    AVL = inserir(AVL, 39);
    AVL = inserir(AVL, 13);
    AVL = inserir(AVL, 22);
    AVL = inserir(AVL, 32);
    AVL = inserir(AVL, 34);
    AVL = inserir(AVL, 36);
    AVL = inserir(AVL, 40);
    AVL = inserir(AVL, 37);

    imprimir_in_order(AVL, 0);
    printf("\n\nApos remoção da chave 1\n\n");
    AVL = remover(AVL, 1);

    imprimir_in_order(AVL, 0);
    liberar_arvore(AVL);
  }

  {
    printf("\n\nExercicio 3\n\n");

    Arvore<std::string> *AVL = nullptr;

    AVL = inserir<std::string>(AVL, "Pedro");
    AVL = inserir<std::string>(AVL, "Joaquim");
    AVL = inserir<std::string>(AVL, "Sandra");
    AVL = inserir<std::string>(AVL, "Daniele");
    AVL = inserir<std::string>(AVL, "Jhony");
    AVL = inserir<std::string>(AVL, "Bruna");
    AVL = inserir<std::string>(AVL, "Roberto");
    AVL = inserir<std::string>(AVL, "Ricardo");
    AVL = inserir<std::string>(AVL, "Rafael");
    AVL = inserir<std::string>(AVL, "Hayana");
    AVL = inserir<std::string>(AVL, "Lorena");
    AVL = inserir<std::string>(AVL, "Mariana");
    AVL = inserir<std::string>(AVL, "Marina");
    AVL = inserir<std::string>(AVL, "Roberta");
    AVL = inserir<std::string>(AVL, "Chayene");
    AVL = inserir<std::string>(AVL, "Edmundo");

    imprimir_in_order(AVL, 0);
    liberar_arvore(AVL);
  }

  return 0;
}
