#include <stdio.h>

int main()
{
    int km, litros;
    float consumoMedio;

    printf("Digite a distância percorrida (em km) e total de combustível gasto (em litros), separados por vírgula: ");
    scanf("%d,%d", &km, &litros);

    consumoMedio = (float)km / litros;

    printf("O consumo médio foi de %.3f km/l\n", consumoMedio);

    return 0;
}