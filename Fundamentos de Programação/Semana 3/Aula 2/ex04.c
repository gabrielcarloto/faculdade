#include <stdio.h>

int main()
{
    int intNum;
    float floatNum;
    char character;

    printf("Digite um número inteiro: ");
    scanf("%d", &intNum);
    printf("Digite um caractere: ");
    scanf(" %c", &character);
    printf("Digite um número de ponto flutuante: ");
    scanf("%f", &floatNum);

    printf("Você digitou: %d, %c e %.2f\n", intNum, character, floatNum);

    return 0;
}