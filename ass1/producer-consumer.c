#include <errno.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "ass1_rand.h"

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 32
#endif

#ifndef NUM_PRODUCERS
#define NUM_PRODUCERS 3
#endif

#ifndef NUM_CONSUMERS
#define NUM_CONSUMERS 3
#endif

struct item {
    int value;
    unsigned int wait_period;
};

struct item Buffer[BUFFER_SIZE];
size_t BufferFill = 0;
pthread_mutex_t BufferLock = PTHREAD_MUTEX_INITIALIZER;

void *producer() {
    while (true) {
        // Sleep for some amount of time
        unsigned int sleep_length = get_random_production_period();

#ifdef DEBUG
        fprintf(stderr, "Producer sleeping for %u\n", sleep_length);
#endif
        (void)sleep(sleep_length);

        // Produce the item
        struct item born;
        born.value = get_random_number();
        born.wait_period = (get_random_number() % 7) + 2;

        // Acquire the lock
        while (pthread_mutex_lock(&BufferLock));

        // Check to see if the buffer is full
        if (BufferFill == BUFFER_SIZE) {
            // The buffer is full. Give back the buffer.
            pthread_mutex_unlock(&BufferLock);
            continue;
        }
        // Set the new item
        Buffer[BufferFill] = born;

#ifdef DEBUG
        fprintf(stderr, "Producing value %zu\n", BufferFill);
#endif
        // Indicate the item has been produced
        BufferFill++;
        // Release the lock
        pthread_mutex_unlock(&BufferLock);
    }
    return NULL;
}

void *consumer() {
    while (true) {
        // Acquire the lock.
        while (pthread_mutex_lock(&BufferLock));
        // If the buffer is empty, release the lock and retry.
        if (BufferFill == 0) {
            pthread_mutex_unlock(&BufferLock);
            continue;
        }
        // Now that the lock has been acquired and we know there is at least 1
        // item in the buffer consume the last item.
        BufferFill--;

#ifdef DEBUG
        size_t current_buffer_fill = BufferFill;

        fprintf(stderr, "Start consuming value %zu\n", current_buffer_fill);
#endif

        struct item eaten = Buffer[BufferFill];
        // Unlock release the lock now that the item has been consumed.
        pthread_mutex_unlock(&BufferLock);


        // Sleep for the amount of time indicated in the item.
#ifdef DEBUG
        fprintf(stderr, "Consumer sleeping for %u\n", eaten.wait_period);
#endif
        (void)sleep(eaten.wait_period);
        printf("%d\n", eaten.value);

#ifdef DEBUG
        fprintf(stderr, "Finish consuming value %zu\n", current_buffer_fill);
#endif

    }
    return NULL;
}

int main() {

    int err;

    pthread_t consumer_pool[NUM_CONSUMERS];
    pthread_t producer_pool[NUM_PRODUCERS];

    random_number_init();

    for (int i = 0; i < NUM_PRODUCERS; ++i) {
        while ((err = pthread_create(&producer_pool[i], NULL, producer, NULL))) {
            if (err == EAGAIN) {
                continue;
            } else { // EINVAL
                exit(EXIT_FAILURE);
            }
            //pthread_join(producer_pool[i], NULL);
        }
    }

    for (int i = 0; i < NUM_CONSUMERS; ++i) {
        while ((err = pthread_create(&consumer_pool[i], NULL, consumer, NULL))) {
            if (err == EAGAIN) {
                continue;
            } else { // EINVAL
                exit(EXIT_FAILURE);
            }
        }
    }
    while(true){}
    return 0;
}
