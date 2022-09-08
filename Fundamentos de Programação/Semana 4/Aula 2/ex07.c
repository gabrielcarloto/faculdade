#include <stdio.h>

int main()
{
    int aux;

    printf("Digite um valor inteiro: ");
    scanf("%d", &aux);
    printf("O valor inserido em formato de ponto flutuante é: %.2f\n", (float)aux);

    return 0;
}