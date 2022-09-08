#include <stdio.h>

int main()
{
    int n1, n2;
    float media;

    printf("Digite um valor inteiro: ");
    scanf("%d", &n1);
    printf("Digite outro valor inteiro: ");
    scanf("%d", &n2);

    media = ((float)n1 + n2) / 2;

    printf("A media entre os valores é igual a %.2f\n", media);

    return 0;
}