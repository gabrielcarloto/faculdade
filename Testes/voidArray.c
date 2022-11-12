#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 5

typedef enum
{
  INT,
  CHAR,
  FLOAT,
} Type;

void *allocArray(int size, Type t);
void printArray(void *v, Type t, int size);
void fillArray(void *v, Type t, int size);

int main()
{
  int *intV;
  char *charV;
  float *floatV;

  intV = allocArray(SIZE, INT);
  charV = allocArray(SIZE, CHAR);
  floatV = allocArray(SIZE, FLOAT);

  printArray(intV, INT, SIZE);
  printArray(charV, CHAR, SIZE);
  printArray(floatV, FLOAT, SIZE);

  // ? it works, but should I use it?

  return 0;
}

void *allocArray(int size, Type t)
{
  void *v;

  switch (t)
  {
  case INT:
    v = (int *)malloc(size * sizeof(int));
    fillArray(v, INT, size);
    break;
  case CHAR:
    v = (char *)malloc(size * sizeof(char));
    fillArray(v, CHAR, size);
    break;
  case FLOAT:
    v = (float *)malloc(size * sizeof(float));
    fillArray(v, FLOAT, size);
    break;
  }

  if (v == NULL)
    exit(1);

  return v;
}

void fillArray(void *v, Type t, int size)
{
  size_t i;

  if (t == INT)
  {
    int *vet = (int *)v;
    for (i = 0; i < size; i++)
      vet[i] = 42;
  }
  else if (t == CHAR)
  {
    char *vet = (char *)v;
    strcpy(v, "auaua");
  }
  else if (t == FLOAT)
  {
    float *vet = (float *)v;
    for (i = 0; i < size; i++)
      vet[i] = 42.69;
  }
}

void printArray(void *v, Type t, int size)
{
  size_t i;

  if (t == INT)
  {
    int *vet = (int *)v;
    for (i = 0; i < size; i++)
      printf("%d, ", vet[i]);
    printf("\n");
  }
  else if (t == CHAR)
  {
    char *vet = (char *)v;
    printf("%s\n", vet);
  }
  else if (t == FLOAT)
  {
    float *vet = (float *)v;
    for (i = 0; i < size; i++)
      printf("%.2f, ", vet[i]);
    printf("\n");
  }
}