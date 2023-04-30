# Round Robin Scheduling Simulation

Program simulates round-robin scheduling for given input file of processes.

## Building

Build the program by running `make` with the provided Makefile.

This program was tested on Linux kernel [release](https://kernel.org) 5.14.8-arch1-1 and also on MacOS Ventura 13.3.1. You can additionally verify this by running `python -m unittest`.

## Documentation of Assumptions

- scheduler will break arrival time ties by list ordering
- one unit of time is atomic (indivisible)
- processes produce a response immediately upon execution
- there is no context switch time
- the scheduler is "tickless" and runs on a "one shot" hardware timer -- if a process terminates in the middle of a time slice, the next timer interrupt will be generated at `now() + quantum_length`
- the currently running process will always be queued after those that arrive at the time its quanta is over

## Running the Simulation

### Input File

See the attached file `processes.txt` for an example of the accepted format. The outline is as follows, where anything between `[ ]` must be an unsigned integer:

```
[number of processes to schedule]
[first process pid], [first process arrival time], [first process burst time]
...
[last process pid], [last process arrival time], [last process burst time]

```

Note that it is not necessary for the list to be ordered by arrival time.

### Executing the Program

Simply perform the following command within your favorite shell:

```
$ ./rr [path to input file] [time quantum]
```

and the program will display the average waiting and response times.

## Cleaning up

Clean up all build-related files by running `make clean`.
