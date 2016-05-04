CC=gcc
CFLAGS= -std=c99 -O0 -Wall -Werror -Wextra -pthread
DEBUG_CFLAGS=-g

all: cme

debug: CFLAGS += $(DEBUG_CFLAGS)
debug: all

cme: cme.c random.o randr.o
	$(CC) cme.c random.o randr.o -o cme $(CFLAGS)

random.o: random.c
	$(CC) -c random.c -o random.o $(CFLAGS)

randr.o: randr.S
	$(CC) -c randr.S -o randr.o $(CFLAGS)

clean:
	rm -f cme *.o
	rm -rf *.dSYM

.PHONY: clean all debug
