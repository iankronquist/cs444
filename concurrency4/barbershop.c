#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <pthread.h>
#include <semaphore.h>

#define MAX_HAIRCUT_TIME 3
#define NUM_NAMES 5

char *first_names[NUM_NAMES] = {
    "Louis ",
    "Ella ",
    "Nat ",
    "Duke ",
    "Dave ",
};

char *last_names[NUM_NAMES] = {
    "Armstrong",
    "Fitzgerald",
    "King Cole",
    "Ellington",
    "Brubeck",
};

pthread_cond_t customers_available;
pthread_cond_t barber_available;

pthread_mutex_t customers_available_lock;
pthread_mutex_t barber_lock;

pthread_mutex_t chairs_lock;
sem_t chairs;

void balk() {
    pthread_exit(NULL);
}

int rand_haircut_time() {
    return  2 + (rand() % MAX_HAIRCUT_TIME);
}

void get_haircut() {
    sleep(rand_haircut_time());
}

void cut_hair() {
    sleep(1);
}

void *customer(void *nameptr) {
    char *name = nameptr;
    int num_free_chairs;
#ifdef DEBUG
    printf("*%s walks into the shop*\n", name);
#endif
    // If all the chairs are occupied, balk
    pthread_mutex_lock(&chairs_lock);
    sem_getvalue(&chairs, &num_free_chairs);
    if (num_free_chairs == 0) {
        pthread_mutex_unlock(&chairs_lock);
        printf("%s: Oh my god, would you just look at the wait!\n", name);
        balk();
    }

    printf("%s: I'll just take a seat.\n", name);
    // Sit down.
    sem_wait(&chairs);
    pthread_mutex_unlock(&chairs_lock);

    // Signal that you're ready.
#ifdef DEBUG
    printf("%s: *signals*\n", name);
#endif
    pthread_cond_signal(&customers_available);
#ifdef DEBUG
    printf("%s: *locks*\n", name);
#endif
    pthread_mutex_lock(&barber_lock);
#ifdef DEBUG
    printf("%s: *waits*\n", name);
#endif
    // Wait for the barber.
    pthread_cond_wait(&barber_available, &barber_lock);
    pthread_mutex_unlock(&barber_lock);

    printf("%s: Just a little trim on the sides.\n", name);
    get_haircut();

    printf("%s: Thank you so much, it looks gorgeous.\n", name);
    // Get up from your chair.
    pthread_mutex_lock(&chairs_lock);
    sem_post(&chairs);
    pthread_mutex_unlock(&chairs_lock);

    free(nameptr);

    return NULL;
}

void *barber() {
    while (true) {
        printf("Barber: *yawns*\n");
        pthread_mutex_lock(&customers_available_lock);
#ifdef DEBUG
        printf("Barber: *locks*\n");
#endif
        // Snooze while waiting for the next customer.
        pthread_cond_wait(&customers_available, &customers_available_lock);
        pthread_mutex_unlock(&customers_available_lock);
        printf("Barber: Next!\n");
        pthread_cond_signal(&barber_available);
        cut_hair();
        printf("Barber: Well there you go!\n");
    }
}

char *get_rand_name() {
    char *first = first_names[rand() % NUM_NAMES];
    char *last = last_names[rand() % NUM_NAMES];
    char *full_name = malloc(strlen(first) + strlen(last) + 1);
    strcpy(full_name, first);
    strcat(full_name, last);
    return full_name;
}

void spawn_loop() {
    while (true) {
        pthread_t thrd;
        pthread_create(&thrd, NULL, customer, get_rand_name());
        pthread_detach(thrd);
        sleep(rand_haircut_time());
    }
}

int main() {
    srand(time(NULL));
    pthread_mutex_init(&chairs_lock, NULL);
    pthread_mutex_init(&customers_available_lock, NULL);
    pthread_mutex_init(&barber_lock, NULL);
    sem_init(&chairs, 0, 5);
    pthread_cond_init(&barber_available, NULL);
    pthread_cond_init(&customers_available, NULL);
    pthread_t barber_thrd;
    pthread_create(&barber_thrd, NULL, barber, NULL);
    spawn_loop();
    return pthread_join(barber_thrd, NULL);
}
