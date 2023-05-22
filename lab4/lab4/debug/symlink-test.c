#include <stdio.h>
#include <string.h>

int main () {
  unsigned int i[3] = {0};
  /*
  i[0] = 0x6c6c6568;
  i[1] = 0x6f772d6f;
  i[2] = 0x00646c72;
  */

  memcpy(&i[0], "hell", 4);
  memcpy(&i[1], "o-wo", 4);
  memcpy(&i[2], "rld", 3);
  printf("%.*s\n", 11, &i);
}
