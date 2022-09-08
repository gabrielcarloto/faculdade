#include <stdio.h>

int main()
{
  const float peso1 = 2, peso2 = 3, peso3 = 5;
  float nota1, nota2, nota3, media;

  printf("Digite suas tres notas, separadas por virgula, entre 0 e 10: ");
  scanf("%f,%f,%f", &nota1, &nota2, &nota3);

  media = (nota1 * peso1 + nota2 * peso2 + nota3 * peso3) / (peso1 + peso2 + peso3);

  printf("A sua media eh %.1f.\n", media);

  return 0;
}