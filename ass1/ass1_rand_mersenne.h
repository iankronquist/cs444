#ifndef ASS1_RAND_MERSENNE_H
#define ASS1_RAND_MERSENNE_H
#include <stdint.h>
#include <stddef.h>

#define N 624
#define M 397
#define F 1812433253
#define R 31
#define A 0x9908B0DF
#define B 0x9D2C5680
#define C 0xEFC60000
#define D 0xFFFFFFFF
#define U 11
#define S 7
#define T 15
#define L 18
#define W 32
#define LOWER_MASK 0xffffffff
#define UPPER_MASK (~LOWER_MASK)

void initialize_mersenne_twister(uint32_t seed);

uint32_t mersenne_twister_get_random_number();

#endif
