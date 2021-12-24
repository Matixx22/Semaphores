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

//sem_t* mutex;
//sem_t* empty;
//sem_t* full;

pthread_t prod_thread1, prod_thread2, cons_thread1;

struct Queue* queues[Q_NUM];

void producer(int item, int tid) {
	int id = rand() % Q_NUM;
	
	//sem_wait(empty);
	//sem_wait(mutex);

	enqueue(queues[id], item);
	printf("Producer id %d produced item: %d to queue: %d\n", tid, item, id);

	//sem_post(mutex);
	//sem_post(full);
}

int consumer(int tid) {
	int item;
	int id = rand() % Q_NUM;

	//sem_wait(full);
	//sem_wait(mutex);

	// check if queue is empty
	while (isEmpty(queues[id])) {
		printf("Changed queue from %d to %d\n", id, (id+1)%Q_NUM);
		id = (id+1) % Q_NUM;
		sleep(1);
	}

	item = dequeue(queues[id]);
	printf("Consumer id %d consumed item: %d from queue: %d\n", tid, item, id);

	//sem_post(mutex);
	//sem_post(empty);

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

void* cons() {
	int i = 0, j, k;
	while (i < 12) {
		consumer(pthread_self());
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

	//mutex = make_semaphore(1);
	//empty = make_semaphore(Q_CAP);
	//full = make_semaphore(0);

	pthread_create(&prod_thread1, NULL, prod1, NULL);
	pthread_create(&prod_thread2, NULL, prod2, NULL);
	pthread_create(&cons_thread1, NULL, cons, NULL);

	pthread_join(prod_thread1, NULL);
	pthread_join(prod_thread2, NULL);
	pthread_join(cons_thread1, NULL);

	return 0;
}
