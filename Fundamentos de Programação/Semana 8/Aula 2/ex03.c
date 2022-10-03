#include <stdio.h>

int testaTipoChar(char c);

int main()
{
  char c;

  printf("Digite qualquer coisa: ");
  scanf("%c", &c);

  printf("%d", testaTipoChar(c));

  return 0;
}

int testaTipoChar(char c)
{
  int asciiCode = c;

  if (asciiCode >= 65 && asciiCode <= 90)
    return 1;
  else if (asciiCode >= 97 && asciiCode <= 122)
    return 2;
  else if (asciiCode >= 48 && asciiCode <= 57)
    return 3;
  else
    return 0;
}