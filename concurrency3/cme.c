#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#include <pthread.h>

#include "random.h"

#define MAX 100
#define SLEEP_PERIOD 1
#define A_WHILE 60
#define NUM_SEARCHERS 3
#define NUM_INSERTERS 2
#define NUM_DELETERS 2

struct node {
    int value;
    struct node *next;
};

struct llist {
    pthread_mutex_t insert, delete;
    struct node *head, *tail;
} List;


void list_init(struct llist *list) {
    pthread_mutex_t blank = PTHREAD_MUTEX_INITIALIZER;
    list->head = NULL;
    list->tail = NULL;
    list->insert = blank;
    list->delete = blank;
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

void insert_head(struct llist *l, int item) {
    struct node *new;
restart:
    new = make_node(item, l->head);
    if (new == NULL) {
        printf("Cannot allocate memory."
               " Not performing any insertions for a while");
        sleep(A_WHILE);
        goto restart;
    }
    pthread_mutex_lock(&l->insert);
    l->head = new;
    pthread_mutex_unlock(&l->insert);
}

bool search(struct llist *l, int needle) {
    struct node *straw = l->head;
    while (straw) {
        if (straw->value == needle) {
            return true;
        }
        straw = straw->next;
    }
    return false;
}

void remove_tail(struct llist *l) {
    pthread_mutex_lock(&l->delete);
    if (l->tail != NULL) {
        if (l->tail == l->head) {
            pthread_mutex_lock(&l->insert);
            struct node *old = l->tail;
            l->tail = old->next;
            free(old);
            l->head = NULL;
            l->tail = NULL;
            pthread_mutex_unlock(&l->insert);
        } else {
            struct node *old = l->tail;
            l->tail = old->next;
            free(old);
        }
    }
    pthread_mutex_unlock(&l->delete);
}

void *inserter(void *idhack) {
    uintptr_t id = (uintptr_t)idhack;
    while (true) {
        int value = get_random_number() % MAX;
        insert_head(&List, value);
        printf("[%lu]: Inserted the value %d at the head.\n", id, value);
        sleep(SLEEP_PERIOD);
    }
    return NULL;
}

void *deleter(void *idhack) {
    uintptr_t id = (uintptr_t)idhack;
    while (true) {
        remove_tail(&List);
        printf("[%lu]: Removed the value from the tail.\n", id);
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
    pthread_t searchers[NUM_SEARCHERS], deleters[NUM_DELETERS],
              inserters[NUM_INSERTERS];
    // Start workers
    for (unsigned long i = 0; i < NUM_INSERTERS; ++i) {
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
