## UID: 205577156

# A Kernel Seedling

Create virtual file /proc/count to display number of active processes on machine.

## Building

Run `make` on the command-line with the provided Makefile present in the same directory to build the module.

## Running

Run
```
$ sudo insmod proc_count.ko
```
to insert the module. You may then run
```
$ cat /proc/count
```
at any time to get the number of active processes on your machine.

## Cleaning Up

Simply run
```
$ rmmod proc_count
```
to remove the module and unmount `/proc/count`. Please note this only works because there are no dependencies.

Run `make clean` to get rid of build-related files within this directory.

## Testing

Running
```
$ python -m unittest
```
displays that all test cases correctly passed.

This module was tested on:
> 5.14.8-arch1-1
as seen by running `uname -r` on the command-line.

Report which kernel release version you tested your module on
(hint: use `uname`, check for options with `man uname`).
It should match release numbers as seen on <https://www.kernel.org/>.
