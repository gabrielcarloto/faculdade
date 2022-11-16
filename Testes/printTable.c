#include <stdio.h>
#include <stdarg.h>
#include <string.h>

// https://stackoverflow.com/a/35706345
#define printTable(...) printT(__VA_ARGS__, NULL)

void printT(const char *str, ...);
int maxChars(const char *firstArg, va_list args);

int main()
{
  printTable("asdasdasdasdasda", "Opaaa", "Testando", "Muy buenooooo!!! :)");

  return 0;
}

void printT(const char *str, ...)
{
  va_list list, list2;
  int i, len, tableLen, strMaxChars;

  va_start(list, str);
  va_copy(list2, list);

  strMaxChars = maxChars(str, list);

  va_end(list);

  tableLen = strMaxChars + 4;

  for (i = 0; i < tableLen; i++)
    printf("=");

  printf("\n");

  while (str)
  {
    printf("= %s%*s =\n", str, strMaxChars - strlen(str), "");
    str = va_arg(list2, char *);
  }

  va_end(list2);

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