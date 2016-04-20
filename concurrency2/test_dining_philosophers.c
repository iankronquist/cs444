#include "dining_philosophers.h"
#include "tests.h"

#include <assert.h>

void lock_range(int begin, int end) {
    for (int i = begin; i < end; ++i) {
        pthread_mutex_lock(&Locks[i]);
    }
}

void unlock_range(int begin, int end) {
    for (int i = begin; i < end; ++i) {
        pthread_mutex_unlock(&Locks[i]);
    }
}

bool is_locked(int lock) {
    int status = pthread_mutex_trylock(&Locks[lock]);
    if (status == 0) {
        pthread_mutex_unlock(&Locks[lock]);
        return false;
    } else if (status == EBUSY) {
        return true;
    } else {
        fprintf(stderr, "Broken lock!\n");
        exit(EXIT_FAILURE);
    }
}

bool is_unlocked(int lock) {
    return !is_locked(lock);
}

bool are_all(int begin, int end, bool (*f)(int)) {
    for (int i = begin; i < end; ++i) {
        if (!f(i)) {
            return false;
        }
    }
    return true;
}

int main() {
    int philo0 = 0;
    int philo2 = 2;

    // Initial condition
    EXPECT_EQ(are_all(0, NUM_FORKS, is_unlocked), true);

    get_chopsticks(philo0);

    EXPECT_EQ(are_all(0, 2, is_locked), true);
    EXPECT_EQ(are_all(2, NUM_FORKS, is_unlocked), true);

    put_chopsticks(philo0);

    EXPECT_EQ(are_all(0, NUM_FORKS, is_unlocked), true);

    get_chopsticks(philo0);

    EXPECT_EQ(are_all(0, 2, is_locked), true);
    EXPECT_EQ(are_all(2, NUM_FORKS, is_unlocked), true);

    get_chopsticks(philo2);

    EXPECT_EQ(are_all(0, 4, is_locked), true);
    EXPECT_EQ(are_all(4, NUM_FORKS, is_unlocked), true);

    put_chopsticks(philo0);
    put_chopsticks(philo2);
    EXPECT_EQ(are_all(0, NUM_FORKS, is_unlocked), true);

    return RETURN_VALUE;
}
