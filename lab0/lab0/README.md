# A Kernel Seedling

Create virtual file /proc/count to display number of active processes on machine.

## Building

Run `make` on the command-line with the provided Makefile to build the module.

## Running

Run:
```
$ sudo insmod proc_count.ko
```
to insert the module. You may then run:
```
$ cat /proc/count
```
at any time to see the number of active processes on your machine.

## Cleaning Up

Simply run:
```
$ sudo rmmod proc_count
```
to remove the module and unmount `/proc/count`. Please note that this only works because there are no dependencies.

Run `make clean` to get rid of build-related files.

## Testing

Running:
```
$ python -m unittest
```
shows that all test cases correctly passed.

This module was tested on Linux kernel [release](https://kernel.org) 5.14.8-arch1-1, as seen by running `uname -r` on the command-line.
