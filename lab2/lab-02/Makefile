CFLAGS = -std=gnu17 -Wpedantic -Wall -O0 -pipe -fno-plt -fPIC
ifeq ($(shell uname -s),Darwin)
	LDFLAGS =
else
	LDFLAGS = -lrt -Wl,-O1,--sort-common,--as-needed,-z,relro,-z,now
endif

.PHONY: all
all: rr

rr: rr.o

tar:
	@tar -cf 205577156-lab2-submission.tar rr.c README.md
.PHONY: clean
clean:
	rm -f rr.o rr
