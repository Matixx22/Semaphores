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

#define Q_CAP 100
#define Q_NUM 10 

pthread_t prod_thread1, prod_thread2, cons_thread1, cons_thread2;

struct Queue* queues[Q_NUM];

void prod_alarm() {
	int i;
	for (i = 0; i < Q_NUM; ++i) {
		enqueue(queues[i], INT_MAX);
	}
	printf("Alarm produced\n");
}

void producer(int item, int tid) {
	int id = rand() % Q_NUM;

	// while there is a full queue choose queue with index++ until find not full queue
	while (isFull(queues[id])) {
		printf("Producer id %d waiting... Changing queue from %d to %d\n", tid, id, (id+1)%Q_NUM);
		id = (id+1) % Q_NUM;
		sleep(1);	
	}

	if (front(queues[id]) == INT_MAX) return;

	enqueue(queues[id], item);
	printf("Producer id %d produced item: %d to queue: %d\n", tid, item, id);
}

int consumer(int tid) {
	int item;
	int id = rand() % Q_NUM;

	// if queue is empty choose queue with index++ until find not empty queue
	while (isEmpty(queues[id])) {
		printf("Consumer id %d waiting... Changing queue from %d to %d\n", tid, id, (id+1)%Q_NUM);
		id = (id+1) % Q_NUM;
		sleep(1);
	}
	item = dequeue(queues[id]);
	if (item == INT_MAX) {
		printf("Alarm received by consumer %d from queue %d. Clearing queues...\n", tid, id);	
		return(INT_MAX);
	}

	printf("Consumer id %d consumed item: %d from queue: %d\n", tid, item, id);

	return(item);
}

void* prod1() {
	int i = 0, j, k;
	while (i < 10) {
		producer(i, pthread_self());
		++i;
		for (j = 0; j < 10; ++j) { for (k = 0; k < 9999999; ++k) {} }
	}
}

void* prod2() {
	int i = 0, j, k;
	while (i < 15) {
		producer(i, pthread_self());
		++i;
		for (j = 0; j < 20; ++j) { for (k = 0; k < 9999999; ++k) {} }
	}
	prod_alarm();
}

void* cons1() {
	int item;
	int i = 0, j, k;

	while (i < 100) {
		item = consumer(pthread_self());
		if (item == INT_MAX) {
			pthread_exit(NULL);
		}
		++i;
		for (j = 0; j < 1; ++j) { for (k = 0; k < 9999999; ++k) {} }
	}
}

void* cons2() {
	int item;
	int i = 0, j, k;

	while (i < 40) {
		item = consumer(pthread_self());
		if (item == INT_MAX) {
			pthread_exit(NULL);	
		}
		++i;
		for (j = 0; j < 3; ++j) { for (k = 0; k < 9999999; ++k) {} }
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
