OBJS = pipe.o

CFLAGS = -std=c17 -Wpedantic -Wall -Wshadow -Wextra -Werror -O2 -pipe -fno-plt
ifeq ($(shell uname -s),Darwin)
	LDFLAGS =
else
	LDFLAGS = -Wl,-O1,--sort-common,--as-needed,-z,relro,-z,now
endif

FILENAME=README.md
UID = $(shell cat ${FILENAME} | grep '## UID'| grep -oe '\([0-9.]*\)')

pipe: ${OBJS}

tar:
	@tar -cf ${UID}-lab1-submission.tar pipe.c README.md
.PHONY: clean
clean:
	@rm -f ${OBJS} pipe
check: pipe
	[ "$$(./pipe ls cat cat cat head)" = "$$(ls | cat | cat | cat | head)" ]
