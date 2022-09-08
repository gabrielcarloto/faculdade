#include <stdio.h>

int main()
{
    int base, altura, resultado;

    printf("Calculadora de perímetro do retângulo\n");
    printf("Digite o valor da base: ");
    scanf("%d", &base);
    printf("Digite o valor da altura: ");
    scanf("%d", &altura);

    resultado = base * 2 + altura * 2;

    printf("O perímetro é igual a %d\n", resultado);

    return 0;
}