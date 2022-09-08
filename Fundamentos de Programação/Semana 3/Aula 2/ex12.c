#include <stdio.h>
#include <stdlib.h>

int main()
{
    int a, b, c, min;

    printf("Digite um valor inteiro: ");
    scanf("%d", &a);
    printf("Digite outro inteiro: ");
    scanf("%d", &b);
    printf("Digite mais um inteiro: ");
    scanf("%d", &c);

    min = (a + b - abs(a - b)) / 2;
    min = (min + c - abs(min - c)) / 2;

    printf("O menor valor é %d\n", min);

    return 0;
}