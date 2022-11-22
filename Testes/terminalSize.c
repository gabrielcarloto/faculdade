#include <stdio.h>
#include <windows.h>

typedef struct
{
  int width;
  int height;
} Terminal_Size;

void getTerminalSize(Terminal_Size *tsize);

int main()
{
  Terminal_Size tsize;

  system("cls");
  getTerminalSize(&tsize);

  for (size_t i = 0; i < tsize.height; i++)
  {
    for (size_t j = 0; j < tsize.width; j++)
    {
      printf("%d", i % 10);
    }
    printf("\n");
  }
}

void getTerminalSize(Terminal_Size *tsize)
{
  CONSOLE_SCREEN_BUFFER_INFO csbi;

  // https://stackoverflow.com/a/12642749
  GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
  tsize->width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
  tsize->height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}