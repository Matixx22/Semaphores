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
#define Q_NUM 2

sem_t* mutex;
sem_t* empty;
sem_t* full;

pthread_t prod_thread1, prod_thread2, cons_thread1;

struct Queue* queues[Q_NUM];

void perror_exit(char *s) {
  	perror(s);  
  	exit(-1);
}

void *check_malloc(int size) {
  	void *p = malloc(size);
  	if (p == NULL) perror_exit("malloc failed");
  	return p;
}

sem_t* make_semaphore(int value) {
  	sem_t* sem = check_malloc(sizeof(sem_t));
  	int n = sem_init(sem, 0, value);
  	if (n != 0) perror_exit("sem_init failed");
  	return sem;
}

void producer(int item, int tid) {
	srand(time(NULL));
	int queue_id = rand() % Q_NUM;
	
	sem_wait(empty);
	sem_wait(mutex);

	enqueue(queues[queue_id], item);
	printf("Producer id %d produced item: %d to queue: %d\n", tid, item, queue_id);

	sem_post(mutex);
	sem_post(full);
}

int consumer(int tid) {
	int item;
	srand(time(NULL));
	int queue_id = rand() % Q_NUM;

	sem_wait(full);
	sem_wait(mutex);

	item = dequeue(queues[queue_id]);
	printf("Consumer id %d consumed item: %d from queue: %d\n", tid, item, queue_id);

	sem_post(mutex);
	sem_post(empty);

	return(item);
}

void* prod1() {
	int i = 0, j, k;
	while (i < 6) {
		producer(i, pthread_self());
		++i;
		//sleep(1);
		for (j = 0; j < 10; ++j) { for (k = 0; k < 9999999; ++k) {} }
	}
}

void* prod2() {
	int i = 0, j, k;
	while (i < 4) {
		producer(i, pthread_self());
		++i;
		//sleep(2);
		for (j = 0; j < 20; ++j) { for (k = 0; k < 9999999; ++k) {} }
	}
}

void* cons() {
	int i = 0, j, k;
	while (i < 8) {
		consumer(pthread_self());
		++i;
		//sleep(3);
		for (j = 0; j < 30; ++j) { for (k = 0; k < 9999999; ++k) {} }
	}
}

int main(int argc, char** argv) {
	int i;
	int item;
	
	// init queues
	for (i = 0; i < Q_NUM; ++i) {
		queues[i] = createQueue(Q_CAP, i);
	}

	mutex = make_semaphore(1);
	empty = make_semaphore(Q_CAP);
	full = make_semaphore(0);

	pthread_create(&prod_thread1, NULL, prod1, NULL);
	pthread_create(&prod_thread2, NULL, prod2, NULL);
	pthread_create(&cons_thread1, NULL, cons, NULL);

	pthread_join(prod_thread1, NULL);
	pthread_join(prod_thread2, NULL);
	pthread_join(cons_thread1, NULL);

	/*
	if (fork() == 0) {
		printf("I am a producer\n");
		for (i = 0; i < 20; ++i) {
			producer(q, i);
			sleep(1);
		}
	
		return(0);
	}

	if (fork() == 0) {
		printf("I am a consumer\n");
		for (j = 0; j < 10; ++j) {
			consumer(q);
			sleep(1);
		}
		return(0);
	}
	*/

	return 0;
}
