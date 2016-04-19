#include "dining_philosophers.h"

void interrupted(int sig_num) {
    cleanup(sig_num);
}

int main() {
    char *sem_name = strcat(getlogin(), "_philosophers");
    signal(SIGINT, interrupted);
    Chopsticks = sem_open(sem_name, O_CREAT, 0700, NUM_FORKS);
    if (Chopsticks == SEM_FAILED) {
        perror("Opening semphore");
        exit(EXIT_FAILURE);
    }
    for (unsigned i = 0; i < NUM_PHILOSOPHERS; ++i) {
        pthread_create(&Workers[i], NULL, i);
    }
    for (unsigned i = 0; i < NUM_PHILOSOPHERS; ++i) {
        pthread_join(Workers[i], NULL);
    }
    cleanup(EXIT_SUCCESS);
    // NOT REACHED
    return 0;
}
