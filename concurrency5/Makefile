CC=gcc
CFLAGS= -std=c99 -O2 -Wall -Werror -Wextra -pthread

all: smokers

smokers: smokers.c
	$(CC) smokers.c -o smokers $(CFLAGS)


clean:
	rm -f smokers

.PHONY: clean all
