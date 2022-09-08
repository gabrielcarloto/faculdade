#include <stdio.h>

int main()
{
    int i1, i2, divis, rest;

    printf("Digite um valor inteiro: ");
    scanf("%d", &i1);
    printf("Digite outro valor inteiro: ");
    scanf("%d", &i2);

    divis = i1 / i2;
    rest = i1 % i2;

    printf("A divisão de %d por %d é igual a %d e sobra %d\n", i1, i2, divis, rest);

    return 0;
}