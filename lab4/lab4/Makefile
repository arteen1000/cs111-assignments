ifeq ($(shell uname -s),Darwin)
	CFLAGS = -std=gnu17 -Wall -O0 -pipe -fno-plt -fPIC
	LDFLAGS =
else
	CFLAGS = -std=gnu17 -Wall -O0 -pipe -fno-plt -fPIC
	LDFLAGS = -lrt -pthread -Wl,-O1,--sort-common,--as-needed,-z,relro,-z,now
endif

FILENAME=README.md
UID = $(shell cat ${FILENAME} | grep '\#\# UID'| grep -oe '\([0-9.]*\)')
SUBMISSION_FILES = ext2-create.c README.md
.PHONY: all
all: ext2-create

ext2-create: ext2-create.o

dump-fs: ext2-create
	./ext2-create
	dumpe2fs cs111-base.img

img: ext2-create
	./ext2-create

mnt: img
	mkdir mnt
	sudo mount -o loop cs111-base.img mnt

umnt:
	sudo umount mnt
	rmdir mnt

dump-bits: debug/debug-bits.c img
	$(CC) debug/debug-bits.c -o debug-bits
	./debug-bits

tar:
	@tar -cf ${UID}-lab4-submission.tar ${SUBMISSION_FILES}
.PHONY: clean
clean:
	rm -f ext2-create.o ext2-create
	rm -f *.img *~
	rm -f ${UID}-lab4-submission.tar
	rm -f debug-bits a.out
