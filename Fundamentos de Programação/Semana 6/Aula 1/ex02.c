#include <stdio.h>

int main()
{
  int positiveNumsCount = 0;
  float userNum, average = 0;

  printf("Digite quantos numeros quiser\n");
  printf("No final, voce sabera quantos sao positivos e qual eh a media deles.\n");
  printf("Digite -1000 para parar.\n");

  while (userNum != -1000)
  {
    scanf("%f", &userNum);

    if (userNum > 0)
    {
      positiveNumsCount++;
      average += userNum;
    }
  }

  average = average / positiveNumsCount;

  printf("Voce digitou %d numeros positivos.\n", positiveNumsCount);
  printf("A media entre eles eh %.1f.\n", average);

  return 0;
}