# Linux Ext2 File System Initializer

Initialize a `ext2` file system image in `cs111-base.img`.

## Building

Run `make` to create an executable file `ext2-create`. You can then run

```
$ ./ext2-create
```

to create the file system image `cs111-base.img`.

You can run `fsck.ext2 cs111-base.img` to check the file system and `dumpe2fs cs111-base.img` to dump information on the file system.

In addition, you can run `python -m unittest` to verify that it passes all unit tests.

## Running

Here's a working example of mounting and viewing the file system:

```
$ make && ./ext2-create
$ mkdir mnt
$ sudo mount -o loop cs111-base.img mnt/
$ ls -ain mnt/
total 7
      2 drwxr-xr-x 3    0    0 1024 May 22 08:44 .
1081967 drwxr-xr-x 5 1000 1000 4096 May 22 08:47 ..
     13 lrw-r--r-- 1 1000 1000   11 May 22 08:44 hello -> hello-world
     12 -rw-r--r-- 1 1000 1000   12 May 22 08:44 hello-world
     11 drwxr-xr-x 2    0    0 1024 May 22 08:44 lost+found
$ cat mnt/hello mnt/hello-world
Hello world
Hello world
```

## Cleaning up

You can unmount the file system and remove the directory by running:

```
$ sudo umount mnt && rmdir mnt
```

You can also run `make clean` to get rid of all binary files.

## OS Platform

Tested and verified on Linux release `5.14.8-arch1-1`.