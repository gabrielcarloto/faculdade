#include <stdio.h>

int main()
{
    float n1, n2, aux;

    printf("Digite o primeiro valor: ");
    scanf("%f", &n1);
    printf("Digite o segundo valor: ");
    scanf("%f", &n2);

    // se eliminar os `scanf`, os valores serão apenas lixo da memória,
    // apresentados como 0.00 por conta da formatação do `printf`, e
    // o resultado será 0

    aux = n1 + n2;

    printf("A soma dos dois valores é igual a %.2f\n", aux);
    printf("Os valores digitados foram: %.2f e %.2f\n", n1, n2);

    return 0;
}