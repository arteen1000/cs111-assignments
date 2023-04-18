## UID: 205577156
(IMPORTANT: Only replace the above numbers with your true UID, do not modify spacing and newlines, otherwise your tarfile might not be created correctly)

## Pipe Up

Similar functionality to the UNIX Pipe `|` operator.

## Building

Build the program by running `make` with the provided Makefile.

## Running

The program is meant to have similar functionality to the UNIX Pipe so that `ls | wc | cat` is the equivalent of `./pipe ls wc cat`.

All arguments to the command must be command names or paths to executables. The program will terminate early with relevant exit status if
any system call fails or any of the sub-commands have non-zero status.

The program will return an error code if run with no arguments:

```
$ ./pipe
```

For a single argument, the command specified will run with stdin and stdout unchanged:

```
$ ./pipe ls
Makefile pipe pipe.o pipe.c README.md test_lab1.py
```

For multiple arguments, the first command will take input from stdin and the last will output to stdout, with intermediary inputs and outputs being passed down the pipe:

```
$ ./pipe ls wc cat
9 9 86
```

## Cleaning up

Clean up all build files by running `make clean`.
