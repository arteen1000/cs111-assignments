/* Copyright 2023 Arteen Abrishami */

#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/queue.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

typedef uint32_t u32;
typedef int32_t i32;

struct process
{
  u32 pid;
  u32 arrival_time;
  u32 burst_time;

  TAILQ_ENTRY(process) pointers;

  /* Additional fields here */
  /* End of "Additional fields here" */
};

TAILQ_HEAD(process_list, process);

// read in the next unsigned integer from data (can include non-integers)
u32 next_int(const char **data, const char *data_end)
{
  u32 current = 0;
  bool started = false;
  while (*data != data_end)
  {
    char c = **data;

     // if not a digit
    if (c < 0x30 || c > 0x39)
    {
      if (started)
      {
        return current;
      }
    }
    else
    {
      if (!started)
      {
        // digit as an int
        current = (c - 0x30);
        started = true;
      }
      else
      {
        // get the int correctly if consecutive digits
        current *= 10;
        current += (c - 0x30); 
      }
    }

    ++(*data);
  }

  printf("Reached end of file while looking for another integer\n");
  exit(EINVAL);
}

// C string to integer, exit if bad value (non-digit) anywhere
u32 next_int_from_c_str(const char *data)
{
  char c;
  u32 i = 0;
  u32 current = 0;
  bool started = false;
  while ((c = data[i++]))
  {
    if (c < 0x30 || c > 0x39)
    {
      exit(EINVAL);
    }
    if (!started)
    {
      current = (c - 0x30);
      started = true;
    }
    else
    {
      current *= 10;
      current += (c - 0x30);
    }
  }
  return current;
}


void init_processes(const char *path,
                    struct process **process_data,
                    u32 *process_size)
{
  // expect path to processes.txt or other to be first argument 
  int fd = open(path, O_RDONLY);
  if (fd == -1)
  {
    int err = errno;
    perror("open");
    exit(err);
  }

  // return info into 'st' of open file known by 'fd'
  struct stat st;
  if (fstat(fd, &st) == -1)
  {
    int err = errno;
    perror("stat");
    exit(err);
  }

  // file size, in bytes
  u32 size = st.st_size;

  // map the data from the fd to the region that you return to data_start
  const char *data_start = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
  if (data_start == MAP_FAILED)
  {
    int err = errno;
    perror("mmap");
    exit(err);
  }

  // grab address of 'data_start' and 'data_end'
  const char *data_end = data_start + size;
  const char *data = data_start;


  // first int represents numbers of proccesses to expect
  *process_size = next_int(&data, data_end); 

  // get enough mem. rep for the # of the processes
  *process_data = calloc(sizeof(struct process), *process_size);
  if (*process_data == NULL)
  {
    int err = errno;
    perror("calloc");
    exit(err);
  }

  // get the pid, the arrival time, the burst time for each process in our contig. array
  for (u32 i = 0; i < *process_size; ++i)
  {
    (*process_data)[i].pid = next_int(&data, data_end);
    (*process_data)[i].arrival_time = next_int(&data, data_end);
    (*process_data)[i].burst_time = next_int(&data, data_end);
  }

  // free the mem map and close fd
  munmap((void *)data, size);
  close(fd);
}


int main(int argc, char *argv[])
{
  // req. 3 args
  if (argc != 3)
  {
    return EINVAL;
  }

  // init process data and size from first argument
  struct process *data;
  u32 size;
  init_processes(argv[1], &data, &size);

  // get quantum length from second argument
  u32 quantum_length = next_int_from_c_str(argv[2]);

  // init process list with TAILQ, may do this different
  struct process_list list;
  TAILQ_INIT(&list);

  // init values to increment and return
  u32 total_waiting_time = 0;
  u32 total_response_time = 0;

  /* Your code here */
  
  /* End of "Your code here" */

  printf("Average waiting time: %.2f\n", (float)total_waiting_time / (float)size);
  printf("Average response time: %.2f\n", (float)total_response_time / (float)size);

  free(data);
  return 0;
}
