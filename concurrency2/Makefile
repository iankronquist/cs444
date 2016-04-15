CC=gcc
CFLAGS= -std=c99 -O2 -Wall -Werror -Wextra -pthread -Wno-deprecated-declarations

all: dining_philosophers test_dining_philosophers

dining_philosophers: dining_philosophers.c main.c
	$(CC) main.c dining_philosophers.c -o dining_philosophers $(CFLAGS)

test_dining_philosophers: test_dining_philosophers.c dining_philosophers.c
	$(CC) dining_philosophers.c test_dining_philosophers.c -o test_dining_philosophers $(CFLAGS)

clean:
	rm -f dining_philosophers test_dining_philosophers *.o

.PHONY: clean all
