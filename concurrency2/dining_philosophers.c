#include "dining_philosophers.h"

// Philosophers living as of 04/14/2016
char *Philosophers[NUM_PHILOSOPHERS] = {
    "Peter Singer",
    "Noam Chomsky",
    "Jürgen Habermas",
    "Saul Kripke",
    "Martha Nussbaum",
};

pthread_t Workers[NUM_PHILOSOPHERS];

sem_t *Chopsticks;

void think() {
    int sleep_time = (rand() % 20) + 1;
    sleep(sleep_time);
}

void eat() {
    int sleep_time = (rand() % 8) + 2;
    sleep(sleep_time);
}

void get_chopsticks() {
    if (sem_wait(Chopsticks) == -1) {
        perror("Getting first chopstick");
        cleanup(EXIT_FAILURE);
    }
    if (sem_trywait(Chopsticks) == -1) {
        if (errno == EAGAIN || errno == EINTR) {
            sem_post(Chopsticks);
        } else {
            perror("Getting second chopstick");
            cleanup(EXIT_FAILURE);
        }
    }
}

void put_chopsticks() {
    if (sem_post(Chopsticks) == -1) {
        perror("Posting first chopstick");
        cleanup(EXIT_FAILURE);
    }
    if (sem_post(Chopsticks) == -1) {
        perror("Posting first chopstick");
        cleanup(EXIT_FAILURE);
    }
}

void cleanup(int exit_val) {
    if (sem_close(Chopsticks) != 0) {
        perror("Closing semaphore");
        exit(EXIT_FAILURE);
    }
    exit(exit_val);
}

void *loop(void *input) {
    char *name = input;
    while (true) {
        printf("%s is starting to think.\n", name);
        think();
        printf("%s is done thinking.\n", name);
        get_chopsticks();
        printf("%s is starting to eat.\n", name);
        eat();
        printf("%s is done eating.\n", name);
        put_chopsticks();
    }
    return NULL;
}
