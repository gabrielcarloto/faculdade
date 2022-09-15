#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void delay(secs)
{
  int milli_secs = secs * 1000;
  clock_t start = clock();
  while (clock() < start + milli_secs)
    ;
}

int main()
{
  int randNum, userAttempt, attemptsCount;
  time_t t;

  srand(time(&t));

  randNum = rand() % 501;

  printf("Tente descobrir o numero escolhido entre 1 e 500: ");
  scanf("%d", &userAttempt);
  attemptsCount = 1;

  while (userAttempt != randNum)
  {
    if (attemptsCount >= 10)
      printf("\033[30;1mEi! O numero eh %d\033[0m\n", randNum);

    printf("Tentativa errada. Tente novamente: ");
    scanf("%d", &userAttempt);
    attemptsCount++;
  }

  printf("Tentativa e");

  delay(1);

  printf("\b \b");
  delay(0.01);

  printf("EXATA!\n");
  printf("Voce tentou %d vezes ", attemptsCount);

  if (attemptsCount >= 1 && attemptsCount <= 3)
    printf("\\o/");
  else if (attemptsCount >= 4 && attemptsCount <= 6)
    printf(":-D");
  else if (attemptsCount >= 7 && attemptsCount <= 10)
    printf(":-)");
  else if (attemptsCount > 10)
    printf(":-\\");

  return 0;
}