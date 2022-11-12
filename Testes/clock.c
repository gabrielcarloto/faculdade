#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_CHARS 100

#define HIDE_CURSOR "\e[?25l"
#define SHOW_CURSOR "\e[?25h"
#define ERASE_LINE "\x1b[2K"
#define BEGGINING_PREV_LINE "\x1b[1F"

void intToDay(int weekDay, char *str);
void intToMonth(int month, char *str);
void delay(int secs);
void showCursor();

int main()
{
  time_t start = time(0);
  struct tm *info = localtime(&start);
  int s = info->tm_sec, m = info->tm_min, h = info->tm_hour, d = info->tm_mday, wday = info->tm_wday, mon = info->tm_mon;
  char currentTime[MAX_CHARS], weekDay[20], month[20];

  atexit(showCursor);
  intToDay(wday, weekDay);
  intToMonth(mon, month);
  printf("%s", HIDE_CURSOR);

  while (1)
  {
    snprintf(currentTime, MAX_CHARS, "%s, %d de %s\n%.2d:%.2d:%.2d", weekDay, d, month, h, m, s);
    printf("%s", currentTime);
    s += 1;

    if (s == 60)
    {
      s = 0;
      m += 1;
    }

    if (m == 60)
    {
      m = 0;
      h += 1;
    }

    if (h == 24)
    {
      h = 0;
    }

    delay(1);
    printf("%s%s%s", ERASE_LINE, BEGGINING_PREV_LINE, ERASE_LINE);
    // for (size_t i = 0; i < strlen(currentTime); i++)
    //   printf("\b");
  }

  return 0;
}

void intToDay(int weekDay, char *str)
{
  if (weekDay == 0)
    strcpy(str, "Domingo");
  else if (weekDay == 1)
    strcpy(str, "Segunda-feira");
  else if (weekDay == 2)
    strcpy(str, "Terça-feira");
  else if (weekDay == 3)
    strcpy(str, "Quarta-feira");
  else if (weekDay == 4)
    strcpy(str, "Quinta-feira");
  else if (weekDay == 5)
    strcpy(str, "Sexta-feira");
  else if (weekDay == 6)
    strcpy(str, "Sábado");
}

void intToMonth(int month, char *str)
{
  if (month == 0)
    strcpy(str, "janeiro");
  else if (month == 1)
    strcpy(str, "fevereiro");
  else if (month == 2)
    strcpy(str, "março");
  else if (month == 3)
    strcpy(str, "abril");
  else if (month == 4)
    strcpy(str, "maio");
  else if (month == 5)
    strcpy(str, "junho");
  else if (month == 6)
    strcpy(str, "julho");
  else if (month == 7)
    strcpy(str, "agosto");
  else if (month == 8)
    strcpy(str, "setembro");
  else if (month == 9)
    strcpy(str, "outubro");
  else if (month == 10)
    strcpy(str, "novembro");
  else if (month == 11)
    strcpy(str, "dezembro");
}

void delay(int secs)
{
  int milli_secs = secs * 1000;
  clock_t start = clock();
  while (clock() < start + milli_secs)
    ;
}

void showCursor()
{
  printf("%s", SHOW_CURSOR);
}