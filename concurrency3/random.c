#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>

#include "random.h"

extern uint32_t cpu_supports_rdrand();
extern uint32_t native_get_random_number();

static unsigned int (*real_get_random_number)(void);

static unsigned int stdlib_rand_stub(void) {
    return (unsigned int)rand();
}

unsigned int get_random_number() { return real_get_random_number(); }

void random_number_init()
{
	if (cpu_supports_rdrand()) {
        puts("rdrand!");
		real_get_random_number = native_get_random_number;
	} else {
        puts("rand!");
        srand(time(0));
		real_get_random_number = stdlib_rand_stub;
	}
}
