#include <stdint.h>
#include <time.h>

#include "ass1_rand.h"
#include "ass1_rand_mersenne.h"

extern uint32_t cpu_supports_rdrand();
extern uint32_t native_get_random_number();

static unsigned int (*real_get_random_number)(void);

unsigned int get_random_number() { return real_get_random_number(); }

// Get a random production period in the integer range [3,7).
unsigned int get_random_production_period()
{
	return (get_random_number() % 4) + 3;
}

// Get a random waiting period in the integer range [2,9).
unsigned int get_random_waiting_period()
{
	return (get_random_number() % 7) + 2;
}

void random_number_init()
{
	if (cpu_supports_rdrand()) {
		real_get_random_number = native_get_random_number;
	} else {
		initialize_mersenne_twister(time(0));
		real_get_random_number = mersenne_twister_get_random_number;
	}
}
