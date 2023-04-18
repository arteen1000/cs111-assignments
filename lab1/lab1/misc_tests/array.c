#include <stdio.h>

int main () {
  int array[5] = { 0 };

  printf("value of array: %p\n", array);
  printf("value of &array: %p\n", &array);
  printf("value of &array[0]: %p\n", &array[0]);

  int *pointer = array;

  printf("value of pointer: %p\n", pointer);
  printf("value of &pointer: %p\n", &pointer);
  printf("value of &pointer[0]: %p\n", &pointer[0]);

  return 0;
}

