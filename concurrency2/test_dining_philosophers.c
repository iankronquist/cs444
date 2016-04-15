#include "dining_philosophers.h"
#include "tests.h"

#include <assert.h>

int main() {
    int sem_val;
    char *sem_name = strcat(getlogin(), "_philosophers_test");
    Chopsticks = sem_open(sem_name, O_CREAT, 0700, NUM_FORKS);
    assert(Chopsticks != SEM_FAILED);

    // When there are sufficient chopsticks

    // Initial condition
    sem_getvalue(Chopsticks, &sem_val);
    EXPECT_EQ(sem_val, 0);

    get_chopsticks();

    sem_getvalue(Chopsticks, &sem_val);
    EXPECT_EQ(sem_val, 2);

    put_chopsticks();

    sem_getvalue(Chopsticks, &sem_val);
    EXPECT_EQ(sem_val, 0);


    assert(sem_close(Chopsticks) == 0);

    return 0;
}
