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
#include "queue.h"

#define Q_CAP 10
#define Q_NUM 3

pthread_t prod_thread1, prod_thread2, cons_thread1, cons_thread2;

struct Queue* queues[Q_NUM];

void producer(int item, int tid) {
	int id = rand() % Q_NUM;

	enqueue(queues[id], item);
	printf("Producer id %d produced item: %d to queue: %d\n", tid, item, id);
}

int consumer(int tid) {
	int item;
	int id = rand() % Q_NUM;

	// check if queue is empty
	while (isEmpty(queues[id])) {
		printf("Consumer id %d waiting... Changed queue from %d to %d\n", tid, id, (id+1)%Q_NUM);
		id = (id+1) % Q_NUM;
		sleep(1);
	}

	item = dequeue(queues[id]);
	printf("Consumer id %d consumed item: %d from queue: %d\n", tid, item, id);

	return(item);
}

void* prod1() {
	int i = 0, j, k;
	while (i < 6) {
		producer(i, pthread_self());
		++i;
		for (j = 0; j < 10; ++j) { for (k = 0; k < 9999999; ++k) {} }
	}
}

void* prod2() {
	int i = 0, j, k;
	while (i < 4) {
		producer(i, pthread_self());
		++i;
		for (j = 0; j < 20; ++j) { for (k = 0; k < 9999999; ++k) {} }
	}
}

void* cons1() {
	int i = 0, j, k;
	while (i < 10) {
		consumer(pthread_self());
		++i;
		for (j = 0; j < 10; ++j) { for (k = 0; k < 9999999; ++k) {} }
	}
}

void* cons2() {
	int i = 0, j, k;
	while (i < 4) {
		consumer(pthread_self());
		++i;
		for (j = 0; j < 30; ++j) { for (k = 0; k < 9999999; ++k) {} }
	}
}

int main(int argc, char** argv) {
	int i;
	int item;
	srand(time(NULL));	

	// init queues
	for (i = 0; i < Q_NUM; ++i) {
		queues[i] = createQueue(Q_CAP, i);
	}

	pthread_create(&prod_thread1, NULL, prod1, NULL);
	pthread_create(&prod_thread2, NULL, prod2, NULL);
	pthread_create(&cons_thread1, NULL, cons1, NULL);
	pthread_create(&cons_thread2, NULL, cons2, NULL);

	pthread_join(prod_thread1, NULL);
	pthread_join(prod_thread2, NULL);
	pthread_join(cons_thread1, NULL);
	pthread_join(cons_thread2, NULL);

	return 0;
}
