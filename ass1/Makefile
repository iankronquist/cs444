CC=gcc
CFLAGS=-Werror -Wall -Wextra -std=c99 -g


.PHONY: all
all: producer-consumer

producer-consumer: producer-consumer.c ass1_rand.o
	$(CC) producer-consumer.c ass1_rand.o -o producer-consumer $(CFLAGS)

ass1_rand.o: ass1_rand_rdrand.c
	$(CC) -c ass1_rand_rdrand.c -o ass1_rand.o $(CFLAGS)


.PHONY: clean
clean:
	rm -f *.o producer-consumer
