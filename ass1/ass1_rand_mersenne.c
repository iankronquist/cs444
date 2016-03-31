#include "ass1_rand_mersenne.h"

static void twist();

static uint32_t mt[N];
static size_t index = 0;

void initialize_mersenne_twister(uint32_t seed)
{
	mt[0] = seed;
	for (int i = 1; i < N; ++i) {
		mt[i] = F * (mt[i - 1] ^ (mt[i - 1] >> (W - 2))) + i;
	}
}

uint32_t mersenne_twister_get_random_number()
{
	if (index == N) {
		twist();
	}
	int y = mt[index];
	y ^= (y >> U) & D;
	y ^= (y << S) & B;
	y ^= (y >> T) & C;
	y ^= (y >> L);
	index += 1;
	return y;
}

static void twist()
{
	for (size_t i = 0; i < N - 1; ++i) {
		int x = (mt[i] & UPPER_MASK) + (mt[(i + 1) % N] & LOWER_MASK);
		int xA = x >> 1;
		// If x is odd
		if (x & 1) {
			xA ^= A;
		}
		mt[i] = mt[(i + M) % N] ^ xA;
	}
	index = 0;
}
