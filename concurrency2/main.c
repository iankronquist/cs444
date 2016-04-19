#include "dining_philosophers.h"

void interrupted(int sig_num) {
    cleanup(sig_num);
}

int main() {
    signal(SIGINT, interrupted);
    for (unsigned i = 0; i < NUM_PHILOSOPHERS; ++i) {
        void *hack = (unsigned int*)(uintptr_t)i;
        pthread_create(&Workers[i], NULL, loop, hack);
    }
    for (unsigned i = 0; i < NUM_PHILOSOPHERS; ++i) {
        pthread_join(Workers[i], NULL);
    }
    cleanup(EXIT_SUCCESS);
    // NOT REACHED
    return 0;
}
