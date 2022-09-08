#include <stdio.h>
#include <math.h>

int main()
{
    int r;
    float PI = 3.14159, resultado;

    printf("CALCULADORA DE VOLUME DA ESFERA\n\n");

    printf("Digite o valor do raio: ");
    scanf("%d", &r);

    resultado = (float)4 / 3 * PI * pow((double)r, 3);

    printf("RESULTADO: %.3f\n", resultado);

    return 0;
}