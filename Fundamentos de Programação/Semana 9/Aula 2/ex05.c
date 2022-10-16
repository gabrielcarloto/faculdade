#include <stdio.h>

void removeExtremos(int *n, int *pri, int *ult);

int main()
{
  int n, pri, ult, ehPalindromo = 1;

  printf("Verifica palindromo - Digite um numero: ");
  scanf("%d", &n);

  while (n / 10 > 0 && ehPalindromo)
  {
    removeExtremos(&n, &pri, &ult);

    ehPalindromo = pri == ult;
  }

  if (ehPalindromo)
    printf("Eh palindromo.\n");
  else
    printf("Nao eh palindromo.\n");

  return 0;
}

void removeExtremos(int *n, int *pri, int *ult)
{
  int tn, pot;
  *ult = *n % 10;
  pot = 1;
  tn = *n;
  while (tn >= 10)
  {
    tn = tn / 10;
    pot *= 10;
  }
  *pri = *n / pot;
  *n = *n % pot;
  *n = *n / 10;
}