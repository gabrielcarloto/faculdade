#include <stdio.h>
#include <stdlib.h>

int main()
{
    int a, b, c, max;

    printf("Digite um valor inteiro: ");
    scanf("%d", &a);
    printf("Digite outro inteiro: ");
    scanf("%d", &b);
    printf("Digite mais um inteiro: ");
    scanf("%d", &c);

    max = (a + b + abs(a - b)) / 2;
    max = (max + c + abs(max - c)) / 2;

    printf("O maior valor é %d\n", max);

    return 0;
}