# Hash Hash Hash

Base hash table implementation along with two thread-safe implementations.

V1 is interested in correctness only, while V2 is interested in both correctness and performance.

## Building

Simply run:

```
$ make
```

on your favorite shell within a Linux/UNIX environment. The provided `Makefile` will do the rest.

## Running

You may assess the hash table performances using:

```
$ ./hash-table-tester -t [num-threads] -s [num-entries-to-add-per-thread]
```

where the value of `[num-threads]` defaults to 4 and the value of `[num-entries-to-add-per-thread]` defaults to 25000 when run without the corresponding option.

The times given are in μs.

## Implementation Comparison

### hash-table-base

The `hash-table-base.c` file contains a thread-unsafe hash table, which is run single-threaded with `./hash-table-tester`, adding `[num-threads] * [num-entries-to-add-per-thread]` entries to the hash table, as a baseline comparison against the multi-threaded, thread-safe versions.

The implementation is of an open hash table, in which every entry holds a linked list which contains the elements that map to that entry.

### hash-table-v1

In `hash-table-v1.c`, a single coarse grain lock (`pthread_mutex_t`) was introduced within `hash_table_v1`. This was initialized with `pthread_mutex_init` when the hash table was created in `hash_table_v1_create`. The lock was later destroyed using `pthread_mutex_destroy` when the hash table was destroyed in `hash_table_v1_destroy`.

Upon any error returned by the `pthreads` functions, the program simply exits, in conformance of the specification.

As for the actual implementation, the coarse-grain lock was introduced within `hash_table_v1_add_entry`. The lock was locked using `pthread_mutex_lock` before any accesses to the list data structure within an individual hash table entry (the retrieval of the hash table entry itself and the head of the list within were determined thread-safe). The coarse-grain lock was released using `pthread_mutex_unlock` before the function return points. 

The reason for the critical section containing all list-internal reads and writes is that any update by another thread could have race conditions in the access of that given list data structure, and since there is a single lock for the entire hash table, the worst case scenario in which every thread wishes to access the same list must be considered.

### hash-table-v2

In `hash-table-v2.c`, a fine grain locking strategy was used, with one per `hash_table_entry`. These correspond locks on the chained list that the hash table entry holds, with a per-list locking strategy being used. The individual locks were initialized in a looped fashion within `hash_table_v2_create` using `pthread_mutex_init` and destroyed in a similar fashion within `hash_table_v2_destroy` using `pthread_mutex_destroy`.

The actual placements of the locks were exactly the same as `hash-table-v1`; however, the fine grain lock for the particular hash table entry, where the thread is adding an element to, was used. This can be seen in `hash_table_v2_add_entry`. The reason for this is because there are no race conditions with updates to other hash table entries. Race conditions only exist when two threads attempt to add an element to the same hash table entry's internal list.

All errors returned by the `pthreads` library functions were handled with a program exit.

### Performance Analysis

The following was run on a device running `macOS Ventura 13.3.1` with a `2.3 GHz 8-Core Intel Core i9` CPU and `16 GB 2667 MHz DDR4` memory.

```
% ./hash-table-tester -t 8 -s 50000
Generation: 46,778 usec
Hash table base: 648,046 usec
  - 0 missing
Hash table v1: 1,198,078 usec
  - 0 missing
Hash table v2: 71,242 usec
  - 0 missing
% ./hash-table-tester -t 8 -s 50000
Generation: 50,001 usec
Hash table base: 555,665 usec
  - 0 missing
Hash table v1: 1,154,911 usec
  - 0 missing
Hash table v2: 73,273 usec
  - 0 missing
% ./hash-table-tester -t 8 -s 50000
Generation: 48,863 usec
Hash table base: 677,524 usec
  - 0 missing
Hash table v1: 1,732,554 usec
  - 0 missing
Hash table v2: 79,480 usec
  - 0 missing
```

In this case, we use 8 threads, where each of them is responsible for adding 50000 entries to the hash tables. In the case of `hash-table-base`, of course, all 400000 entries are added by a single thread.

We can see that on average, `hash-table-v1` is 2.17 times slower than `hash-table-base`. This is because there is a single coarse-grain lock and a heavy overhead for adding an element to the hash table. There are multiple threads competing for a single lock any time an element is added and the operating system must intervene and schedule them appropriately, for every single addition to the hash table. Thus the cost of context switching, blocking, and scheduling ends up being higher than if the program had just been run single-threaded.

On the other hand, on average, `hash-table-v2` is 8.40 times faster than `hash-table-base`. This is because there is a single fine-grain lock *per* hash table entry, and thus, the operating system must only intervene and ensure proper scheduling only when two threads are experiencing a collision and attempting to add an element to the same hash entry. Since this is an unlikely case, it ends up that the cost of the mutex locks and unlocks becomes amortized over the multi-threaded execution.

Additionally, you can run `python -m unittest` as many times as it please you to find that all unit tests are consistently passed. This was verified on both MacOS and Linux.

## Cleaning up

Simply run:

```
$ make clean
```

within your favorite shell to get rid of all build-related files.
