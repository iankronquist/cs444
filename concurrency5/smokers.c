#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <pthread.h>
#include <semaphore.h>

// From the Little Book of Semaphores
// http://greenteapress.com/semaphores/downey08semaphores.pdf

#define UNUSED(x)  x __attribute__ ((unused))

#define SMOKE_TIME 2
#define IDLE_TIME 1

sem_t tobacco, paper, match, agentSem, tobaccoSem, paperSem, matchSem;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
bool isPaper = false,  isMatch = false, isTobacco = false;

void smoke(void) {
	sleep(SMOKE_TIME);
}

void *smoker_paper(void *UNUSED(_)) {
	while (true) {
		printf("Paper smoker is waiting\n");
		sem_wait(&paperSem);
		printf("Paper smoker is smoking\n");
		smoke();
		printf("Paper is done smoking\n");
		sem_post(&agentSem);
	}
}

void *smoker_match(void *UNUSED(_)) {
	while (true) {
		printf("Match smoker is waiting\n");
		sem_wait(&matchSem);
		printf("Match smoker is smoking\n");
		smoke();
		printf("Match is done smoking\n");
		sem_post(&agentSem);
	}
}

void *smoker_tobacco(void *UNUSED(_)) {
	while (true) {
		printf("Tobacco smoker is waiting\n");
		sem_wait(&tobaccoSem);
		printf("Tobacco smoker is smoking\n");
		smoke();
		printf("Tobacco is done smoking\n");
		sem_post(&agentSem);
	}
}

void agent_a(void) {
	sem_wait(&agentSem);
	printf("Agent puts tobacco on the table\n");
	sem_post(&tobacco);
	printf("Agent puts paper on the table\n");
	sem_post(&paper);
}

void agent_b(void) {
	sem_wait(&agentSem);
	printf("Agent puts paper on the table\n");
	sem_post(&paper);
	printf("Agent puts match on the table\n");
	sem_post(&match);
}

void agent_c(void) {
	sem_wait(&agentSem);
	printf("Agent puts tobacco on the table\n");
	sem_post(&tobacco);
	printf("Agent puts match on the table\n");
	sem_post(&match);
}

void *pusher_a(void *UNUSED(_)) {
	while (true) {
		sem_wait(&tobacco);
		pthread_mutex_lock(&mutex);
		if (isPaper) {
			isPaper = false;
			printf("Pusher A pushes Match smoker\n");
			sem_post(&matchSem);
		} else if (isMatch) {
			isMatch = false;
			printf("Pusher A pushes Paper smoker\n");
			sem_post(&paperSem);
		} else {
			printf("Pusher A got tobacco\n");
			isTobacco = true;
		}
		pthread_mutex_unlock(&mutex);
	}
}

void *pusher_b(void *UNUSED(_)) {
	while (true) {
		sem_wait(&paper);
		pthread_mutex_lock(&mutex);
		if (isTobacco) {
			isTobacco = false;
			printf("Pusher B pushes Match smoker\n");
			sem_post(&matchSem);
		} else if (isMatch) {
			isMatch = false;
			printf("Pusher B pushes Tobacco smoker\n");
			sem_post(&tobaccoSem);
		} else {
			printf("Pusher B got Paper\n");
			isPaper = true;
		}
		pthread_mutex_unlock(&mutex);
	}
}

void *pusher_c(void *UNUSED(_)) {
	while (true) {
		sem_wait(&match);
		pthread_mutex_lock(&mutex);
		if (isTobacco) {
			isTobacco = false;
			printf("Pusher C pushes Paper smoker\n");
			sem_post(&paperSem);
		} else if (isPaper) {
			isPaper = false;
			printf("Pusher C pushes Tobacco smoker\n");
			sem_post(&tobaccoSem);
		} else {
			printf("Pusher C got Match\n");
			isMatch = true;
		}
		pthread_mutex_unlock(&mutex);
	}
}

void *agent_main(void *UNUSED(_)) {
	void (*agents[])(void) = {agent_a, agent_b, agent_c};
	while (true) {
		agents[rand() % 3]();
		sleep(IDLE_TIME);
	}
}

int main() {
	pthread_t agent_thr, pusher_a_thr, pusher_b_thr, pusher_c_thr,
			  smoker_paper_thr, smoker_match_thr, smoker_tobacco_thr;
	srand(time(NULL));
	sem_init(&tobacco, 0, 0);
	sem_init(&paper, 0, 0);
	sem_init(&match, 0, 0);
	sem_init(&agentSem, 0, 1);
	sem_init(&tobaccoSem, 0, 0);
	sem_init(&matchSem, 0, 0);
	sem_init(&paperSem, 0, 0);
	pthread_create(&agent_thr, NULL, agent_main, NULL);

	pthread_create(&smoker_paper_thr, NULL, smoker_paper, NULL);
	pthread_create(&smoker_match_thr, NULL, smoker_match, NULL);
	pthread_create(&smoker_tobacco_thr, NULL, smoker_tobacco, NULL);

	pthread_create(&pusher_a_thr, NULL, pusher_a, NULL);
	pthread_create(&pusher_b_thr, NULL, pusher_b, NULL);
	pthread_create(&pusher_c_thr, NULL, pusher_c, NULL);

	pthread_join(agent_thr, NULL);
}
