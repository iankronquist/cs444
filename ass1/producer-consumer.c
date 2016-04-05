#include <errno.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


#include "_producer-consumer.c"

void *producer()
{
	while (true) {
        _producer();
	}
	return NULL;
}

void *consumer()
{
	while (true) {
        _consumer();
	}
	return NULL;
}

int main()
{

	int err;

	pthread_t consumer_pool[NUM_CONSUMERS];
	pthread_t producer_pool[NUM_PRODUCERS];

	random_number_init();

	for (int i = 0; i < NUM_PRODUCERS; ++i) {
		while ((err = pthread_create(&producer_pool[i], NULL, producer,
					     NULL))) {
			if (err == EAGAIN) {
				continue;
			} else { // EINVAL
				exit(EXIT_FAILURE);
			}
		}
	}

	for (int i = 0; i < NUM_CONSUMERS; ++i) {
		while ((err = pthread_create(&consumer_pool[i], NULL, consumer,
					     NULL))) {
			if (err == EAGAIN) {
				continue;
			} else { // EINVAL
				exit(EXIT_FAILURE);
			}
		}
	}
	for (int i = 0; i < NUM_CONSUMERS; ++i) {
		if (pthread_join(producer_pool[i], NULL)) {
			perror("Main thread joining a producer thread");
			exit(EXIT_FAILURE);
		}
	}
	for (int i = 0; i < NUM_CONSUMERS; ++i) {
		if (pthread_join(consumer_pool[i], NULL)) {
			perror("Main thread joining a consumer thread");
			exit(EXIT_FAILURE);
		}
	}
	return 0;
}
