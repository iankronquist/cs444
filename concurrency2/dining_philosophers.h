#ifndef DINING_PHILOSOPHERS_H
#define DINING_PHILOSOPHERS_H
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#define NUM_PHILOSOPHERS 5
#define NUM_FORKS NUM_PHILOSOPHERS

extern char *Philosophers[NUM_PHILOSOPHERS];
extern pthread_t Workers[NUM_PHILOSOPHERS];
extern sem_t *Chopsticks;

void think();
void eat();
void get_chopsticks();
void put_chopsticks();
void cleanup(int exit_val);
void *loop(void *input);

#endif
