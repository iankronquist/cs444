#include "ass1_rand.h"

unsigned int get_random_period() {
    return (unsigned int)get_random_number();
}

// Get a random production period in the integer range [3,7).
unsigned int get_random_production_period() {
    return (get_random_period() % 4) + 3;
}

// Get a random waiting period in the integer range [2,9).
unsigned int get_random_waiting_period() {
    return (get_random_period() % 7) + 2;
}

int get_random_number() {
    unsigned int rdrand
    volatile __asm__( "rdrand %0" : "=r" (rdrand) );
    return rdrand;
}
