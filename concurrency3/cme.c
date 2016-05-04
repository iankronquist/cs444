#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#include <pthread.h>
#include <semaphore.h>

#include "random.h"

#define MAX 10
#define SLEEP_PERIOD 1
#define A_WHILE 60
#define NUM_SEARCHERS 3
#define NUM_INSERTERS 2
#define NUM_DELETERS 2
#define SEM_MAX (NUM_SEARCHERS+NUM_INSERTERS)

struct node {
    int value;
    sem_t s;
    struct node *next;
};

struct llist {
    sem_t s;
    pthread_mutex_t insert, delete;
    struct node *head, *tail;
} List;


void list_init(struct llist *list) {
    pthread_mutex_t blank = PTHREAD_MUTEX_INITIALIZER;
    list->head = NULL;
    list->tail = NULL;
    list->insert = blank;
    list->delete = blank;
    if(sem_init(&list->s, 0, SEM_MAX) == -1) {
        perror("Initing semaphore");
        exit(EXIT_FAILURE);
    }
}

struct node *make_node(int value, struct node *next) {
    struct node *new = malloc(sizeof(struct node));
    if (new == NULL) {
        return NULL;
    }
    new->next = next;
    new->value = value;
    return new;
}

void insert_tail(struct llist *l, int item) {
    struct node *new;
    sem_wait(&l->s);
restart:
    new = make_node(item, NULL);
    if (new == NULL) {
        printf("Cannot allocate memory."
               " Not performing any insertions for a while");
        sleep(A_WHILE);
        goto restart;
    }
    pthread_mutex_lock(&l->insert);
    if (l->tail != NULL) {
        l->tail->next = new;
    }
    if (l->head == NULL) {
        l->head = new;
    }
    l->tail = new;
    pthread_mutex_unlock(&l->insert);
    sem_post(&l->s);
}

bool search(struct llist *l, int needle) {
    sem_wait(&l->s);
    struct node *straw = l->head;
    while (straw) {
        if (straw->value == needle) {
            sem_post(&l->s);
            return true;
        }
        straw = straw->next;
    }
    sem_post(&l->s);
    return false;
}

void *inserter(void *idhack) {
    uintptr_t id = (uintptr_t)idhack;
    while (true) {
        int value = get_random_number() % MAX;
        insert_tail(&List, value);
        printf("[%lu]: Inserted the value %d at the tail.\n", id, value);
        sleep(SLEEP_PERIOD);
    }
    return NULL;
}

void *deleter(void *idhack) {
    uintptr_t id = (uintptr_t)idhack;
    while (true) {
        int needle = get_random_number() % MAX;
        struct node *prev = List.head;
        struct node *straw = List.head;
        pthread_mutex_lock(&List.delete);
        pthread_mutex_lock(&List.insert);
        for (int i = 0; i < SEM_MAX; ++i) {
            sem_wait(&List.s);
        }
        if (straw != NULL && straw->value == needle) {
            if (straw->next == NULL) {
                List.tail = NULL;
            }
            List.head = straw->next;
            free(straw);
            printf("[%lu]: Removed the value %d from head of list.\n", id,
                   needle);
            goto around;
        }
        while (straw) {
            if (straw->value == needle) {
                prev->next = straw->next;
                if (straw == List.tail) {
                    List.tail = prev;
                }
                free(straw);
                printf("[%lu]: Removed the value %d.\n", id, needle);
                break;
            }
            straw = straw->next;
        }
around:
        for (int i = 0; i < SEM_MAX; ++i) {
            sem_post(&List.s);
        }
        pthread_mutex_unlock(&List.insert);
        pthread_mutex_unlock(&List.delete);
        sleep(SLEEP_PERIOD);
    }
    return NULL;
}

void *searcher(void *idhack) {
    uintptr_t id = (uintptr_t)idhack;
    while (true) {
        int value = get_random_number() % MAX;
        if (search(&List, value)) {
            printf("[%lu]: The list contains %d.\n", id, value);
        } else {
            printf("[%lu]: The list doesn't contain %d.\n", id, value);
        }
        sleep(SLEEP_PERIOD);
    }
    return NULL;
}

int main() {
    random_number_init();
    list_init(&List);
    pthread_t searchers[NUM_SEARCHERS];
    pthread_t deleters[NUM_DELETERS];
    pthread_t inserters[NUM_INSERTERS];
    // Start workers
    for (unsigned long i = 0; i < NUM_INSERTERS; ++i) {
        printf("%lu\n", i);
        pthread_create(&inserters[i], NULL, inserter, (void*)i);
    }
    for (unsigned long i = 0; i < NUM_SEARCHERS; ++i) {
        pthread_create(&searchers[i], NULL, searcher, (void*)i);
    }
    for (unsigned long i = 0; i < NUM_DELETERS; ++i) {
        pthread_create(&deleters[i], NULL, deleter, (void*)i);
    }
    // Wait on workers
    for (unsigned long i = 0; i < NUM_INSERTERS; ++i) {
        pthread_join(inserters[i], NULL);
    }
    for (unsigned long i = 0; i < NUM_SEARCHERS; ++i) {
        pthread_join(searchers[i], NULL);
    }
    for (unsigned long i = 0; i < NUM_DELETERS; ++i) {
        pthread_join(deleters[i], NULL);
    }
    return 0;
}
