#include <stdio.h>
#include <stdarg.h>
#include <string.h>

// https://stackoverflow.com/a/35706345
#define printTable(...) printT(__VA_ARGS__, NULL)

typedef enum
{
  LEFT,
  CENTER,
  RIGHT
} Alignment;

void printT(Alignment alignment, const char *str, ...);
int maxChars(const char *firstArg, va_list args);

int main()
{
  printTable(CENTER, "asdasdasdasdasda", "Opaaa", "Testando", "Muy buenooooo!!! :)");

  return 0;
}

void printT(Alignment alignment, const char *str, ...)
{
  va_list list, list2;
  int i, len, tableLen, strMaxChars, centerAlignSpaces;

  va_start(list, str);
  va_copy(list2, list);

  strMaxChars = maxChars(str, list);

  va_end(list);

  tableLen = strMaxChars + 4;

  for (i = 0; i < tableLen; i++)
    printf("=");

  printf("\n| %*s |\n", strMaxChars, "");

  while (str)
  {
    if (alignment == LEFT)
      printf("= %s%*s =\n", str, strMaxChars - strlen(str), "");
    else if (alignment == CENTER)
    {
      centerAlignSpaces = (strMaxChars - strlen(str)) / 2;
      printf("| %*s%s%*s |\n", centerAlignSpaces, "", str, centerAlignSpaces * 2 + strlen(str) == strMaxChars ? centerAlignSpaces : centerAlignSpaces + 1, "");
    }
    else if (alignment == RIGHT)
      printf("= %*s%s =\n", strMaxChars - strlen(str), "", str);

    str = va_arg(list2, char *);
  }

  va_end(list2);

  printf("| %*s |\n", strMaxChars, "");

  for (i = 0; i < tableLen; i++)
    printf("=");

  printf("\n");
}

int maxChars(const char *firstArg, va_list args)
{
  int len, count = 0;
  char *arg = firstArg;

  while (arg)
  {
    len = strlen(arg);

    if (len > count)
      count = len;

    arg = va_arg(args, char *);
  }

  return count;
}