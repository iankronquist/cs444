CC=gcc
CFLAGS= -std=c99 -O2 -Wall -Werror -Wextra -pthread

all: 3sharable barbershop

3sharable: 3sharable.c
	$(CC) 3sharable.c -o 3sharable $(CFLAGS)

barbershop: barbershop.c
	$(CC) barbershop.c -o barbershop $(CFLAGS)


clean:
	rm -f 3sharable barbershop

.PHONY: clean all
