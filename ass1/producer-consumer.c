#include <bool.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER_SIZE 32
#define NUM_PRODUCERS 3
#define NUM_CONSUMERS 3

struct item {
    int value;
    unsigned int wait_period;
};

struct item Buffer[BUFFER_SIZE] = {0};
size_t BufferFill = 0;
pthread_mutex_t BufferLock = PTHREAD_MUTEX_INITIALIZER;

void producer() {
    while (true) {
        (void)sleep(get_random_period());
        // Acquire the lock
        while (pthread_mutex_lock(&BufferLock)) { }
        // Check to see if the buffer is full
        if (BufferFill == BUFFER_SIZE) {
            // The buffer is full. Give back the buffer.
            pthread_mutex_unlock(&BufferLock);
            continue;
        }
        // Set the new item
        Buffer[BufferFill].value = get_random_number();
        Buffer[BufferFill].wait_period = get_random_period();
        // Indicate the item has been produced
        BufferFill++;
        // Release the lock
        pthread_mutex_unlock(&BufferLock);
    }
}

void consumer() {
    while (true) {
        while (pthread_mutex_lock(&BufferLock)) { }
        if (BufferFill == 0) {
            pthread_mutex_unlock(&BufferLock);
            continue;
        }
        (void)sleep(Buffer[BufferFill-1].wait_period);
        printf("%d\n", Buffer[BufferFill-1].value);
        pthread_mutex_unlock(&BufferLock);
    }
}

int main() {

    int err;

    pthread_t consumer_pool[NUM_CONSUMERS];
    pthread_t producer_pool[NUM_PRODUCERS];

    for (int i = 0; i < NUM_PRODUCERS; ++i) {
        while (err = pthread_create(&producer_pool[i], NULL, producer, NULL)) {
            if (err == EAGAIN) {
                continue;
            } else { // EINVAL
                exit(EXIT_FAILURE);
            }
        }
    }

    for (int i = 0; i < NUM_CONSUMERS; ++i) {
        while (err = pthread_create(&consumer_pool[i], NULL, consumer, NULL)) {
            if (err == EAGAIN) {
                continue;
            } else { // EINVAL
                exit(EXIT_FAILURE);
            }
        }
    }

    return 0;
}
