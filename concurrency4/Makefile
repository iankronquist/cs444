CC=gcc
CFLAGS= -std=c99 -O2 -Wall -Werror -Wextra -pthread

all: 3sharable

3sharable: 3sharable.c
	$(CC) 3sharable.c -o 3sharable $(CFLAGS)

clean:
	rm -f 3sharable

.PHONY: clean all
