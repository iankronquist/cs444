#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <pthread.h>
#include <semaphore.h>

#define NUM_THREADS 5
#define MAX_SLEEP 2

pthread_t Thread_Pool[NUM_THREADS];
char *names[NUM_THREADS] = {
    "Louis Armstrong",
    "Ella Fitzgerald",
    "Nat King Cole",
    "Duke Ellington",
    "Dave Brubeck",
};

/*
 * You are in a coffee shop and after drinking your 10^2 ounce cup of joe you
 * need to use the restroom. There are three restrooms and three keys. On the
 * wall there is a hook with three keys. On the table beside it there is an
 * empty basket. Each person who enters the bathroom takes a key. When they
 * leave the bathroom they put it in the basket. The third person to leave the
 * bathroom puts the other two keys back on the hook along with their own.
 */

struct resource {
    semt_t hook;
    semt_t basket;
} Resource;

// Time it takes to use the resource.
int sleep_time(void) {
    return rand() % MAX_SLEEP;
}

// Acquire the resource, use it, and update bookkeeping.
void get_resource(char *name) {
    printf("%s: I'm gonna wait for a key.\n", name);
    // Take key off hook
    if (sem_wait(&Resource.hook) != 0) {
        perror("Entrance semaphore");
        exit(errno);
    }
    printf("%s: Now that I have the key I'll use the restroom.\n", name);
    // Use resource.
    sleep(sleep_time());
    // Do bookkeeping. If this is the third person leaving the bathroom, take
    // all the keys out of the basket and put them on the hook.
    int val;
    if (sem_getvalue(&Resource.basket, &val) != 0) {
        perror("Counting keys in basket");
        exit(errno);
    }
    if (val == 2) {
        printf("%s: Oh, I guess I'm the last person to leave."
               "Better put all the keys back where they belong.\n", name);
        // Take first key out of basket.
        sem_put(&Resource.basket);
        // Take second key out of basket.
        sem_put(&Resource.basket);

        // Put my key on the hook.
        sem_put(&Resource.hook);
        // Put the first key on the hook.
        sem_put(&Resource.hook);
        // Put the second key on the hook.
        sem_put(&Resource.hook);
        // Go back to reading the paper and listening to smooth jazz.
        // Here:
        // http://www.jazzradio.com/smoothjazz247
        // http://www.nytimes.com/
        return;
    }
    printf("%s: I'll just put my key in the basket\n", name);
    // Put key back in the basket
    if (sem_wait(&Resource.basket) != 0) {
        perror("Putting key in basket");
        exit(errno);
    }
}

// Workers spin, use the resource, and sleep.
void *thread_work(void *idp) {
    intptr_t id = (intptr_t)idp;
    char *name = names[id];
    while (true) {
        // Use the restroom.
        get_resource(name);
        // Drink more coffee.
        sleep(sleep_time());
    }
    return idp;
}

void spawn_threads(void) {
    for (intptr_t i = 0; i < NUM_THREADS; ++i) {
        pthread_create(Thread_Pool[i], NULL, thread_work, (void*)i);
    }
}

int main() {
    // Seed random number generator with the time, because it's easy.
    srand(time(NULL));
    // Initialize the key and hook semaphores.
    if (sem_init(&Resource.hook, 0, 3) == -1) {
        perror("Initializing hook");
        exit(errno);
    }
    if (sem_init(&Resource.basket, 0, 2) == -1) {
        perror("Initializing basket");
        exit(errno);
    }

    // Spawn threads.
    spawn_threads();
    // Spin.
    while (true);
}
