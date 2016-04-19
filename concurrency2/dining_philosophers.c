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
pthread_mutex_t Locks[NUM_PHILOSOPHERS] = {PTHREAD_MUTEX_INITIALIZER};

void think() {
    int sleep_time = (rand() % 20) + 1;
    sleep(sleep_time);
}

void eat() {
    int sleep_time = (rand() % 8) + 2;
    sleep(sleep_time);
}

void get_chopsticks(struct philosopher *philo) {
    while (true) {
        philo->left = NULL;
        philo->right = NULL;
        for (unsigned i = 0; i < NUM_PHILOSOPHERS; ++i) {
            if (pthread_mutex_trylock(&Locks[i]) != 0) {
                if (errno != EBUSY && errno != 0) {
                    perror("Getting chopstick");
                    cleanup(EXIT_FAILURE);
                }
                continue;
            } else {
                printf("%s picked up chopstick %u\n", philo->name, i);
                if (philo->left == NULL) {
                    philo->left = &Locks[i];
                } else {
                    philo->right = &Locks[i];
                    break;
                }
            }

        }
        if (philo->right == NULL) {
            if (philo->left != NULL) {
                printf("%s put down chopstick\n", philo->name);
                pthread_mutex_unlock(philo->left);
            }
            continue;
        }
        break;
    }
}

void put_chopsticks(struct philosopher *philo) {
    if (pthread_mutex_unlock(philo->left) != 0) {
        perror("Unlocking first chopstick");
        cleanup(EXIT_FAILURE);
    }
    if (pthread_mutex_unlock(philo->right) != 0) {
        perror("Unlocking first chopstick");
        cleanup(EXIT_FAILURE);

    }
}

void cleanup(int exit_val) {
    exit(exit_val);
}

void *loop(void *input) {
    unsigned i = (uintptr_t)input;
    struct philosopher *philo = &Philosophers[i];
    philo->name = PhilosopherNames[i];
    while (true) {
        printf("%s is starting to think.\n", philo->name);
        think();
        printf("%s is done thinking.\n", philo->name);
        get_chopsticks(philo);
        printf("%s is starting to eat.\n", philo->name);
        eat();
        printf("%s is done eating.\n", philo->name);
        put_chopsticks(philo);
    }
    return NULL;
}
