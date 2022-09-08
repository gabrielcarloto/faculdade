#include <stdio.h>

int main()
{
  int celsius;
  float fahrenheit;

  printf("Digite a temperatura em celsius: ");
  scanf("%d", &celsius);

  fahrenheit = ((float)celsius / 5) * 9 + 32;

  printf("A temperatura em fahrenheit eh %.1f", fahrenheit);

  return 0;
}