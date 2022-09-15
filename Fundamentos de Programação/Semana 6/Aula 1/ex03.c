#include <stdio.h>

#define PASSWORD 2002

int main()
{
  int userAttempt;

  printf("Digite a senha: ");
  scanf("%d", &userAttempt);

  while (userAttempt != PASSWORD)
  {
    printf("Senha incorreta.\n");
    scanf("%d", &userAttempt);
  }

  printf("Acesso permitido.\n");

  return 0;
}