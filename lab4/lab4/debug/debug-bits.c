#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

/* print binary rep of underlying string */
/* MSb -> LSb for each individual byte */

void
print_binary_rep(unsigned char *buf, size_t size)
{
  size_t i = 0;
  
  while ( i < size)
    {
      unsigned char c = buf[i];

      for (int j = 0 ; j < 8 ; j++)
        {
          (c & ( 0x80 >> j ) ) ? putchar('1') : putchar('0');
        }

      putchar(' ');
      
      i++;
    }
  
  putchar('\n');
  
  return;  
}

#define INODE_BITMAP_BLOCKNO 4
#define BLOCK_SIZE 1024
#define BLOCK_OFFSET(i) (i * BLOCK_SIZE)

int main () {
  int fd;
  if ( (fd = open("cs111-base.img", O_RDONLY)) == -1)
    exit(errno);

  if (lseek(fd, BLOCK_OFFSET(INODE_BITMAP_BLOCKNO), SEEK_SET) == -1)
    exit(errno);

  unsigned char buf[BLOCK_SIZE];

  if (read(fd, buf, BLOCK_SIZE) != BLOCK_SIZE)
    exit(errno);

  print_binary_rep(buf, BLOCK_SIZE);

  return 0;

}

  
