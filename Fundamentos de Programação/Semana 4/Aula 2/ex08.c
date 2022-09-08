#include <stdio.h>

int main()
{
    char aux;

    printf("Digite um caractere: ");
    scanf("%c", &aux);
    printf("O valor deste caractere na tabela ASCII é: %d\n", aux);

    return 0;
}