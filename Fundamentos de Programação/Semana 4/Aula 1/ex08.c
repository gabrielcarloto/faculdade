#include <stdio.h>

#define diaAtual 1
#define mesAtual 9
#define anoAtual 2022

int main()
{
  int dia, mes, ano, idade, idadeAno, diaDaSemana, anoDoSeculo, seculo;

  printf("Digite sua data de nascimento: ");
  scanf("%d/%d/%d", &dia, &mes, &ano);

  idadeAno = ano == anoAtual ? 1 : anoAtual - ano;

  idade = mes < mesAtual
              ? idadeAno
          : mes > mesAtual
              ? idadeAno - 1
          : dia < diaAtual
              ? idadeAno
          : dia == diaAtual
              ? idadeAno - 1
              : idadeAno;

  seculo = ano / 100;
  anoDoSeculo = ano % 100;
  diaDaSemana = (dia + (((mes + 1) * 26) / 10) + anoDoSeculo + (anoDoSeculo / 4) + (seculo / 4) + seculo * 5) % 7 - 1;

  if (dia == diaAtual && mes == mesAtual && idade != 0)
  {
    printf("Feliz aniversario!\n");
  }
  else if (dia == diaAtual && mes == mesAtual && idade == 0)
  {
    printf("Parabens, voce acabou de nascer!\n");
  }

  printf("Agora voce tem %d anos.\n", idade);

  if (diaDaSemana == 0)
  {
    printf("Voce nasceu num domingo.\n");
  }
  else if (diaDaSemana == 1)
  {
    printf("Voce nasceu numa segunda-feira.\n");
  }
  else if (diaDaSemana == 2)
  {
    printf("Voce nasceu numa terca-feira.\n");
  }
  else if (diaDaSemana == 3)
  {
    printf("Voce nasceu numa quarta-feira.\n");
  }
  else if (diaDaSemana == 4)
  {
    printf("Voce nasceu numa quinta-feira.\n");
  }
  else if (diaDaSemana == 5)
  {
    printf("Voce nasceu numa sexta-feira.\n");
  }
  else if (diaDaSemana == 6)
  {
    printf("Voce nasceu num sábado.\n");
  }

  return 0;
}