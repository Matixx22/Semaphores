#include <stdio.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <signal.h>
#include "queue.h"

#define Q_CAP 10
#define Q_NUM 2 
#define P_NUM 2
#define C_NUM 2

pthread_t prod_t[P_NUM];
pthread_t cons_t[C_NUM];

struct Queue* queues[Q_NUM];

void sig_handler(int sig) {
	printf("Consumer id %d received special message. Quiting...\n", gettid());
	pthread_exit(NULL);
}

void prod_alarm(int tid) {
	int i;
	for (i = 0; i < Q_NUM; ++i) {
		printf("Producent id %d proudced special message to queue %d\n", tid, i);
		enqueue(queues[i], INT_MAX);
	}
	//printf("Alarm produced\n");
}

void producer(int item, int tid) {
	int id = rand() % Q_NUM;

	if (front(queues[id]) == INT_MAX) pthread_exit(NULL);
	
	enqueue(queues[id], item);
	printf("Producer id %d produced item: %d to queue: %d\n", tid, item, id);
}

int consumer(int tid) {
	int item;
	int id = rand() % Q_NUM;

	item = dequeue(queues[id]);
	if (item == INT_MAX) {
		//printf("Alarm received by consumer %d from queue %d. Clearing queues...\n", tid, id);	
		return(INT_MAX);
	}
	
	printf("Consumer id %d consumed item: %d from queue: %d\n", tid, item, id);

	return(item);
}

void* prod1() {
	int i = 0, j, k;
	while (i < 5) {
		producer(i, gettid());
		++i;
		for (j = 0; j < 10; ++j) { for (k = 0; k < 9999999; ++k) {} }
	}
}

void* prod2() {
	int i = 0, j, k;
	while (i < 3) {
		producer(i, gettid());
		++i;
		for (j = 0; j < 15; ++j) { for (k = 0; k < 9999999; ++k) {} }
	}
	prod_alarm(gettid());
}

void* cons1() {
	int item;
	int i = 0, j, k;

	while (i < 10) {
		item = consumer(gettid());
		if (item == INT_MAX) {
			// send signal to every consumer thread
			for (j = 0; j < C_NUM; ++j) {
				pthread_kill(cons_t[j], SIGUSR1);
			}
			exit(0);
		}
		++i;
		for (j = 0; j < 15; ++j) { for (k = 0; k < 9999999; ++k) {} }
	}
}

void* cons2() {
	int item;
	int i = 0, j, k;

	while (i < 10) {
		item = consumer(gettid());
		if (item == INT_MAX) {
			// send signal to every consumer thread
			for (j = 0; j < C_NUM; ++j) {
				pthread_kill(cons_t[j], SIGUSR1);
			}
			exit(0);	
		}
		++i;
		for (j = 0; j < 20; ++j) { for (k = 0; k < 9999999; ++k) {} }
	}
}

int main(int argc, char** argv) {
	int i;
	int item;

	srand(time(NULL));	
	signal(SIGUSR1, sig_handler);

	// init queues
	for (i = 0; i < Q_NUM; ++i) {
		queues[i] = createQueue(Q_CAP, i);
	}

	pthread_create(&prod_t[0], NULL, prod1, NULL);
	pthread_create(&prod_t[1], NULL, prod2, NULL);
	pthread_create(&cons_t[0], NULL, cons1, NULL);
	pthread_create(&cons_t[1], NULL, cons2, NULL);

	pthread_join(prod_t[0], NULL);
	pthread_join(prod_t[1], NULL);
	pthread_join(cons_t[0], NULL);
	pthread_join(cons_t[1], NULL);

	return 0;
}
