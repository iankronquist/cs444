CC=gcc
CFLAGS=-Werror -Wall -Wextra -std=c99 -g -lpthread

.PHONY: all
all: producer-consumer

.PHONY: debug
debug: CFLAGS += -g -D DEBUG -D BUFFER_SIZE=4
debug: producer-consumer

producer-consumer: producer-consumer.c ass1_rand.c ass1_rand_mersenne.c ass1_rand.S
	$(CC) producer-consumer.c ass1_rand.c ass1_rand_mersenne.c ass1_rand.S -o producer-consumer $(CFLAGS)

.PHONY: clean
clean:
	rm -f *.o producer-consumer
	rm -rf *.dSYM/
