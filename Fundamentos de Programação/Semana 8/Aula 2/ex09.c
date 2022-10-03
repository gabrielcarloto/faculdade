#include <stdio.h>

float calculaIMC(float altura, float peso);
void imprimeMsgIMC(float imc);

int main()
{
  float peso, altura;

  printf("Digite seu peso e sua altura: ");
  scanf("%f %f", &peso, &altura);

  imprimeMsgIMC(calculaIMC(altura, peso));

  return 0;
}

float calculaIMC(float altura, float peso)
{
  return peso / (altura * altura);
}

void imprimeMsgIMC(float imc)
{
  if (imc < 18.5)
    printf("Voce esta abaixo do peso.\n");
  else if (imc >= 18.5 && imc < 25)
    printf("Voce esta com o peso normal.\n");
  else if (imc >= 25 && imc <= 30)
    printf("Voce esta acima do peso.\n");
  else if (imc > 30)
    printf("Voce esta obeso.\n");
}