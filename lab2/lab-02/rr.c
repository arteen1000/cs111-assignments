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

#define DEBUG

/* scheduling asssumptions
 *
 * response time is always instantaneous upon start of exec
 *
 * using one-shot IRQ Timer, reset if processs finish exec early
 * to full quantum for next running process
 *
 * no context switch time
 *
 * break arrival time breaks on list ordering
 *
 * one unit time is indivisible
 */

struct process
{
  u32 pid;
  u32 arrival_time;
  u32 burst_time;

  struct process *next;

  u32 remaining_time;
  u32 start_exec_time;
  u32 waiting_time;
  u32 response_time;
  
};

struct process_queue {
  struct process *head;
  struct process *trail; 
} sched_queue;

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


  // first int represents numbers of processes to expect
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
    (*process_data)[i].remaining_time =
      (*process_data)[i].burst_time = next_int(&data, data_end);

  }

  // free the mem map and close fd
  munmap((void *)data, size);
  close(fd);
}

void add_to_sched_queue(struct process *data,
                        u32 size,
                        u32 current_arrival_time) {

  for (u32 i = 0; i < size; i++)
    {
    if (data[i].arrival_time == current_arrival_time)
     {
     
      if (!sched_queue.head)
      {
        #ifdef DEBUG
        printf("Process is first in queue: %d\n", data[i].pid);
        #endif
        sched_queue.head = sched_queue.trail = &data[i];
        data[i].next = &data[i];
        #ifdef DEBUG
        printf("set next ptr to %p\n", (void*) &data[i]);
        #endif
      }
      else
        {
        /* newcomers get last, head is their next */
        #ifdef DEBUG
        printf("Process added to queue: %d, %d\n", data[i].pid, i);
        printf("size, current arrival: %d, %d\n", size, current_arrival_time);
        #endif
        data[i].next = sched_queue.head;
        (sched_queue.trail)->next = &data[i];
        sched_queue.trail = &data[i];
        #ifdef DEBUG
        printf("finished adding to queue\n");
        #endif
        }
      printf("outside else statement\n");
     }
    printf("outside if statement\n");
    }
  printf("Leaving add_to_sched_queue\n");
}

void remove_head_from_sched_queue() {
  if (!sched_queue.head) return;
  #ifdef DEBUG
  printf("Removing head from queue\n");
  #endif
  if (sched_queue.head == sched_queue.trail) {
    sched_queue.head = sched_queue.trail = NULL;
    return;
  }
  
  sched_queue.head = (sched_queue.head)->next;
  (sched_queue.trail)->next = sched_queue.head;
}

bool done_scheduling(struct process *data, u32 size) {
  for (u32 i = 0; i < size; i++) {
    if (data[i].remaining_time != 0) {
      #ifdef DEBUGE
      printf("Process with PID: %d has time left\n", data[i].pid);
      #endif
      return false;
    }
  }
  #ifdef DEBUG
  printf("Done scheduling\n");
  #endif
  return true;
}

void increment_waits(u32 amnt) {
  struct process *trav;
  trav = (sched_queue.head)->next;
  while (trav != sched_queue.head) {
    trav->waiting_time += amnt;
  }
}
u32 run_current_head(u32 quantum_length, u32 current_time) {
  if(!sched_queue.head || !sched_queue.trail) return 0;
  if ((sched_queue.head)->remaining_time == (sched_queue.head)->burst_time) {
    (sched_queue.head)->start_exec_time = current_time;
    (sched_queue.head)->response_time = current_time - (sched_queue.head)->arrival_time;
  }
  u32 val;
  if ((val = (sched_queue.head)->remaining_time) < quantum_length) {
    (sched_queue.head)->remaining_time = 0;
    increment_waits(val);
    remove_head_from_sched_queue();
    return val;
  } else if ((sched_queue.head)->remaining_time == quantum_length) {
    (sched_queue.head)->remaining_time = 0;
    increment_waits(quantum_length);
    remove_head_from_sched_queue();
    return quantum_length;
  } else {
    #ifdef DEBUG
    printf("Decreasing by quantum length, >");
    #endif
    (sched_queue.head)->remaining_time -= quantum_length;
    increment_waits(quantum_length);
    return quantum_length;
  }
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
  

  // init values to increment and return
  u32 total_waiting_time = 0;
  u32 total_response_time = 0;

  u32 i = 0;
  u32 j = 0;
  bool run_event = true;
  do
  {
    #ifdef DEBUG
    printf("iteration: %d\n", i);
    #endif
    if (done_scheduling(data,size)) break;
    add_to_sched_queue(data, size, i);
    #ifdef DEBUG
    printf("Leaving add_to_sched_queue");
    #endif

    if (run_event) {
      #ifdef DEBUG
      printf("Entering run_event\n");
      #endif
      j = run_current_head(quantum_length, i);
      j += i;
      run_event = false;
    }
    if (i == j) {
      run_event = true;
      sched_queue.head = (sched_queue.head)->next;
      sched_queue.trail = (sched_queue.trail)->next;
    }

    #ifdef DEBUG
    struct process *trav = sched_queue.head;
    do {
      printf("PID: %d\n", trav->pid);
      printf("Remaining time: %d\n", trav->remaining_time);
      printf("Start exec time: %d\n", trav->start_exec_time);
      printf("Waiting time: %d\n", trav->waiting_time);
      printf("Response time: %d\n", trav->response_time);
      printf("Next ptr: %p\n", (void*) trav->next);
      printf("Self address: %p\n", (void*) trav);
      trav = trav->next;
    } while(trav != sched_queue.head);
    printf("Time unit: %d\n", i);
    printf("run event next cycle: %d\n", run_event);
    #endif
  } while (++i);
  

  printf("Average waiting time: %.2f\n", (float)total_waiting_time / (float)size);
  printf("Average response time: %.2f\n", (float)total_response_time / (float)size);

  free(data);
  return 0;
}
