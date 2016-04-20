#include "dining_philosophers.h"

// Philosophers living as of 04/14/2016
char *PhilosopherNames[NUM_PHILOSOPHERS] = {
    "Peter Singer",
    "Noam Chomsky",
    "Jürgen Habermas",
    "Saul Kripke",
    "Martha Nussbaum",
};

struct philosopher Philosophers[NUM_PHILOSOPHERS];
pthread_t Workers[NUM_PHILOSOPHERS];
pthread_mutex_t Locks[NUM_PHILOSOPHERS] = {
    PTHREAD_MUTEX_INITIALIZER,
    PTHREAD_MUTEX_INITIALIZER,
    PTHREAD_MUTEX_INITIALIZER,
    PTHREAD_MUTEX_INITIALIZER,
    PTHREAD_MUTEX_INITIALIZER
};

void think() {
    int sleep_time = (rand() % 20) + 1;
    sleep(sleep_time);
}

void eat() {
    int sleep_time = (rand() % 8) + 2;
    sleep(sleep_time);
}

void get_chopsticks(int i) {
    if (pthread_mutex_lock(&Locks[i]) != 0) {
        perror("Getting chopstick");
        cleanup(EXIT_FAILURE);
    } else {
        printf("%s picked up left chopstick %u\n", PhilosopherNames[i], i);
    }
    if (pthread_mutex_lock(&Locks[(i+1)%5]) != 0) {
        perror("Getting chopstick");
        cleanup(EXIT_FAILURE);
    } else {
        printf("%s picked up right chopstick %u\n", PhilosopherNames[i],
               (i+1)%5);
    }
}

void put_chopsticks(int i) {
    if (pthread_mutex_unlock(&Locks[i]) != 0) {
        perror("Unlocking first chopstick");
        cleanup(EXIT_FAILURE);
    }
    if (pthread_mutex_unlock(&Locks[(i+1)%5]) != 0) {
        perror("Unlocking second chopstick");
        cleanup(EXIT_FAILURE);
    }
}

void cleanup(int exit_val) {
    exit(exit_val);
}

void *loop(void *input) {
    unsigned i = (uintptr_t)input;
    char *name = PhilosopherNames[i];
    while (true) {
        printf("%s is starting to think.\n", name);
        think();
        printf("%s is done thinking.\n", name);
        get_chopsticks(i);
        printf("%s is starting to eat.\n", name);
        eat();
        printf("%s is done eating.\n", name);
        put_chopsticks(i);
    }
    return NULL;
}
