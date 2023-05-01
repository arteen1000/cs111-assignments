#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

struct process
{
  int pid;
  int arrival_time;
  int burst_time;

  struct process *next;

  int remaining_time;
  int waiting_time;
  int response_time;
};

struct {
  struct process *head;
  struct process *tail;
} sched_queue;

struct process *data;
int size;

int next_int(const char **data, const char *data_end)
{
  int current = 0;
  bool started = false;
  while (*data != data_end)
  {
    char c = **data;

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
        current = (c - 0x30);
        started = true;
      }
      else
      {
        current *= 10;
        current += (c - 0x30);
      }
    }

    ++(*data);
  }

  printf("Reached end of file while looking for another integer\n");
  exit(EINVAL);
}

int next_int_from_c_str(const char *data)
{
  char c;
  int i = 0;
  int current = 0;
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
                    int *process_size)
{
  int fd = open(path, O_RDONLY);
  if (fd == -1)
  {
    int err = errno;
    perror("open");
    exit(err);
  }

  struct stat st;
  if (fstat(fd, &st) == -1)
  {
    int err = errno;
    perror("stat");
    exit(err);
  }

  int size = st.st_size;
  const char *data_start = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
  if (data_start == MAP_FAILED)
  {
    int err = errno;
    perror("mmap");
    exit(err);
  }

  const char *data_end = data_start + size;
  const char *data = data_start;

  *process_size = next_int(&data, data_end);

  *process_data = calloc(sizeof(struct process), *process_size);
  if (*process_data == NULL)
  {
    int err = errno;
    perror("calloc");
    exit(err);
  }

  for (int i = 0; i < *process_size; ++i)
  {
    (*process_data)[i].pid = next_int(&data, data_end);
    (*process_data)[i].arrival_time = next_int(&data, data_end);
    (*process_data)[i].remaining_time = (*process_data)[i].burst_time = next_int(&data, data_end);
    
  }

  munmap((void *)data, size);
  close(fd);
}

bool all_done(void);

void add_to_sched_queue(int);

void remove_head_from_sched_queue(void);

// #define DEBUG
#ifdef DEBUG
void print_out_sched_queue(int current_time) {
  printf("CURRENT TIME: %d\n", current_time);
  printf("---------------------\n");
  printf("sched_queue.head: %p\n", (void*) sched_queue.head);
  printf("sched_queue.tail: %p\n", (void*) sched_queue.tail);
  if (sched_queue.head && sched_queue.tail) {
  struct process *trav = sched_queue.head;
  do {
    printf("pid: %d\n", trav->pid);
    printf("arrival_time: %d\n", trav->arrival_time);
    printf("burst_time: %d\n", trav->burst_time);
    printf("next: %p\n", (void*) trav->next);
    printf("cur: %p\n", (void*) trav);
    printf("remaining_time: %d\n", trav->remaining_time);
    printf("waiting_time: %d\n", trav->waiting_time);
    printf("response_time: %d\n", trav->response_time);
    trav = trav->next;
  } while (trav != sched_queue.head);
  printf("------\n");
  }
}

void print_out_data(void) {
  printf("ALL DATA\n");
  printf("---------------------\n");
  int i = 0;
  struct process *trav = &data[i];
  do {
    printf("pid: %d\n", trav->pid);
    printf("-------\n");    
    printf("arrival_time: %d\n", trav->arrival_time);
    printf("burst_time: %d\n", trav->burst_time);
    printf("next: %p\n", (void*) trav->next);
    printf("cur: %p\n", (void*) trav);
    printf("remaining_time: %d\n", trav->remaining_time);
    printf("waiting_time: %d\n", trav->waiting_time);
    printf("response_time: %d\n", trav->response_time);
    trav = &data[++i];
  } while (trav != &data[size]);
  printf("---------------------\n");
}

#endif



void increment_waits(void);

int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    return EINVAL;
  }
  
  init_processes(argv[1], &data, &size);

  int quantum_length = next_int_from_c_str(argv[2]);
  if (quantum_length == 0) return EINVAL;

  int total_waiting_time = 0;
  int total_response_time = 0;

  int current_time = 0;
  int curr_run = 0;

  add_to_sched_queue(current_time);
  
  do {
    
    if (sched_queue.head)
      
      {

      if (sched_queue.head->remaining_time == sched_queue.head->burst_time)
        sched_queue.head->response_time = current_time - sched_queue.head->arrival_time;
      
      sched_queue.head->remaining_time--;
      curr_run++;
      
#ifdef SAFE
      if (sched_queue.head->remaining_time < 0) {
        fprintf(stderr, "remaining_time < 0");
        exit(1);
      }
#endif

      increment_waits();
      add_to_sched_queue(++current_time);
      
      if (sched_queue.head->remaining_time == 0) {
        curr_run = 0;
        remove_head_from_sched_queue();
      } else if (curr_run == quantum_length) {
        curr_run = 0;
        if ( sched_queue.head != sched_queue.tail ) {
          sched_queue.head = sched_queue.head->next;
          sched_queue.tail = sched_queue.tail->next;
        }
      }
      
      } else add_to_sched_queue(++current_time);
    
#ifdef DEBUG
    print_out_sched_queue(current_time);
#endif    
  } while (!all_done());

  for (int i = 0 ; i < size ; i++) {
    total_response_time += data[i].response_time;
    total_waiting_time += data[i].waiting_time;
  }
  
#ifdef DEBUG
  print_out_data();
#endif
  
  printf("Average waiting time: %.2f\n", (float) total_waiting_time / size);
  printf("Average response time: %.2f\n", (float) total_response_time / size);

  free(data);
  return 0;
}

void increment_waits(void) {
  struct process *trav = sched_queue.head->next;
  while (trav != sched_queue.head) {
    trav->waiting_time++;
    trav = trav->next;
  }
}

bool all_done(void) {
  for (int i = 0 ; i < size ; i++) {
    if (data[i].remaining_time > 0) return false;
  }
  return true;
}

void add_to_sched_queue(int current_time) {

  for (int i = 0 ; i < size ; i++) {

    if (data[i].arrival_time == current_time) {

      if (sched_queue.head == NULL) {
        sched_queue.head = sched_queue.tail = &data[i];
        data[i].next = &data[i];
      }
      else {
        data[i].next = sched_queue.head;
        sched_queue.tail->next =  &data[i];
        sched_queue.tail = &data[i];
      }
    }
  }
}

void remove_head_from_sched_queue(void) {
#ifdef SAFE  
  if (!sched_queue.head || !sched_queue.tail) {
    fprintf(stderr, "trying to remove empty head");
    exit(1);
  }
#endif  
  if (sched_queue.head == sched_queue.tail) {
    sched_queue.head = sched_queue.tail = NULL;
    return;
  }

  sched_queue.head = sched_queue.head->next;
  sched_queue.tail->next = sched_queue.head;
  
}


        
  
  
  
