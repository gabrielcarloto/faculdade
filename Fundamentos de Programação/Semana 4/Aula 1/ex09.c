#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main()
{
  int num1, num2, operacao, resultado, resultadoDoUsuario;
  time_t t, comeco;

  srand(time(&t));

  num1 = rand() % 101;
  num2 = rand() % 101;

  operacao = (num1 + num2) / 40;

  if (operacao == 1)
  {
    resultado = num1 + num2;
    printf("Calcule %d + %d e digite a resposta: ", num1, num2);
  }
  else if (operacao == 2)
  {
    resultado = num1 - num2;
    printf("Calcule %d - %d e digite a resposta: ", num1, num2);
  }
  else if (operacao == 3)
  {
    resultado = num1 * num2;
    printf("Calcule %d * %d e digite a resposta: ", num1, num2);
  }
  else if (operacao == 4)
  {
    resultado = num1 / num2;
    printf("Calcule o quociente da divisao de %d por %d e digite a resposta: ", num1, num2);
  }
  else if (operacao == 5)
  {
    resultado = num1 % num2;
    printf("Calcule o resto da divisao de %d por %d e digite a resposta: ", num1, num2);
  }

  comeco = time(NULL);
  scanf("%d", &resultadoDoUsuario);

  printf("Voca levou %.0fs para fazer a conta ", (double)(time(NULL) - comeco));

  if (resultado == resultadoDoUsuario)
  {
    printf("e a resposta esta correta!\n");
  }
  else
  {
    printf("e a resposta esta incorreta :(\n");
  }

  return 0;
}