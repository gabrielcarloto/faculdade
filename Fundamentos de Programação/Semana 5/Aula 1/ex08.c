#include <stdio.h>

int main()
{
  int i, firstTwoDigits, lastTwoDigits;

  for (i = 1000; i <= 9999; i++)
  {
    firstTwoDigits = i / 100;
    lastTwoDigits = i % 100;

    if ((firstTwoDigits + lastTwoDigits) * (firstTwoDigits + lastTwoDigits) == i)
      printf("%d ", i);
  }

  return 0;
}
