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
#include <stdbool.h>
#include "queue.h"

#define Q_CAP 10
#define Q_NUM 2 
#define P_NUM 2
#define C_NUM 2

pthread_t prod_t[P_NUM];
pthread_t cons_t[C_NUM];

struct Queue* queues[Q_NUM];

bool isAlarm;

void prod_alarm(int tid) {
	printf("Producer %d produced alarm\n", tid);
	isAlarm = true;
}

void producer(int item, int tid) {
	int id = rand() % Q_NUM;
	
	enqueue(queues[id], item);
	printf("Producer id %d produced item: %d to queue: %d\n", tid, item, id);
}

int consumer(int tid) {
	int item;
	int id = rand() % Q_NUM;

	item = dequeue(queues[id]);
	printf("Consumer id %d consumed item: %d from queue: %d\n", tid, item, id);

	return(item);
}

void* prod1() {
	int i = 0, j, k;
	while (i < 5) {
		if (isAlarm) pthread_exit(NULL);

		producer(i, gettid());
		++i;
		for (j = 0; j < 10; ++j) { for (k = 0; k < 9999999; ++k) {} }
	}
	prod_alarm(gettid());
}

void* prod2() {
	int i = 0, j, k;
	while (i < 10) {
		if (isAlarm) pthread_exit(NULL);

		producer(i, gettid());
		++i;
		for (j = 0; j < 15; ++j) { for (k = 0; k < 9999999; ++k) {} }
	}
}

void* cons1() {
	int item;
	int i = 0, j, k;

	while (i < 3) {
		item = consumer(gettid());

		if (isAlarm) {
			printf("Consumer %d received alarm during consuming item %d\n", gettid(), item);
			pthread_exit(NULL);
		}	
		++i;
		for (j = 0; j < 15; ++j) { for (k = 0; k < 9999999; ++k) {} }
	}
}

void* cons2() {
	int item;
	int i = 0, j, k;

	while (i < 5) {
		item = consumer(gettid());

		if (isAlarm) {
			printf("Consumer %d received alarm during consuming item %d\n", gettid(), item);
			pthread_exit(NULL);
		}	
		++i;
		for (j = 0; j < 20; ++j) { for (k = 0; k < 9999999; ++k) {} }
	}
}

int main(int argc, char** argv) {
	int i;
	int item;

	srand(time(NULL));	

	isAlarm = false;

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

	for (i = 0; i < Q_NUM; ++i) {
		printf("queue %d: ", i);
		print_queue(queues[i]);
	}

	return 0;
}
