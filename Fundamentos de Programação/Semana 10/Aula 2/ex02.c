#include <stdio.h>
#include <fcntl.h>
#include <io.h>

#define NL 10
#define NC NL

// https://stackoverflow.com/a/65989147
wchar_t digit_to_superscript(int d);

int main()
{
  int pascal[NL][NC], i, j, aPow, bPow, pasNum;
  _setmode(_fileno(stdout), 0x00020000);

  wprintf(L"Triangulo de Pascal de %d linhas\n\n", NL);

  for (i = 0; i < NL; i++)
  {
    for (j = 0; j <= i; j++)
    {
      if (j != i && j != 0)
        pascal[i][j] = pascal[i - 1][j] + pascal[i - 1][j - 1];
      else
        pascal[i][j] = 1;

      wprintf(L"%d\t", pascal[i][j]);
    }

    wprintf(L"\n");
  }

  // EXTRA

  wprintf(L"\nProduto notavel: ");

  for (j = 0; j < NC; j++)
  {
    pasNum = pascal[NL - 1][j];
    aPow = NC - j - 1;
    bPow = j;

    if (pasNum > 1)
      wprintf(L"%d", pasNum);

    if (aPow > 0)
    {
      wprintf(L"a");

      if (aPow > 1)
        wprintf(L"%lc", digit_to_superscript(aPow));
    }

    if (bPow > 0)
    {
      wprintf(L"b");

      if (bPow > 1)
        wprintf(L"%lc", digit_to_superscript(bPow));
    }

    if (j < NL - 1)
      wprintf(L" + ");
  }

  return 0;
}

wchar_t digit_to_superscript(int d)
{
  wchar_t table[] = {
      // Unicode values
      0x2070,
      0x00B9, // Note that 1, 2 and 3 does not follow the pattern
      0x00B2, // That's because those three were common in various
      0x00B3, // extended ascii tables. The rest did not exist
      0x2074, // before unicode
      0x2075,
      0x2076,
      0x2077,
      0x2078,
      0x2079,
  };

  return table[d];
}