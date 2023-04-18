#include <stdio.h>
#include <unistd.h>

int main (int argc, char *argv[]) {

  pid_t my_pid = getpid();
  char str[30];
  
  sprintf(str, "/proc/%d/fd", my_pid);
  printf("hello, I am gonna print out: %s", str);
  
  //  execvp( "ls", (char *[]) { "ls", str, NULL } );
  
  return 0;
}
