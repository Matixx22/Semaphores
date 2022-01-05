#include <semaphore.h>

#include "queue.h"
#include "sem.h"

struct Queue {
    int front, rear, size, id;
    unsigned capacity;
    int* array;
    sem_t* mutex;
    sem_t* empty;
    sem_t* full;
};
 
struct Queue* createQueue(unsigned capacity, int id) {
    struct Queue* queue = (struct Queue*)malloc(sizeof(struct Queue));
    queue->capacity = capacity;
    queue->front = queue->size = 0;
 
    queue->rear = capacity - 1;
    queue->array = (int*)malloc(queue->capacity * sizeof(int));

    queue->id = id;

    queue->mutex = make_sem(1);
    queue->empty = make_sem(capacity);
    queue->full = make_sem(0);

    return queue;
}
 
int isFull(struct Queue* queue) {
    return (queue->size == queue->capacity);
}
 
int isEmpty(struct Queue* queue) {
    return (queue->size == 0);
}

void enqueue(struct Queue* queue, int item) {
    sem_wait(queue->empty);
    sem_wait(queue->mutex);

    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->array[queue->rear] = item;
    queue->size = queue->size + 1;

    sem_post(queue->mutex);
    sem_post(queue->full);
}
 
int dequeue(struct Queue* queue) {
    sem_wait(queue->full);
    sem_wait(queue->mutex);

    int item = queue->array[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size = queue->size - 1;

    sem_post(queue->mutex);
    sem_post(queue->empty);

    return item;
}
 
int front(struct Queue* queue) {
    if (isEmpty(queue))
        return INT_MIN;
    return queue->array[queue->front];
}
 
int rear(struct Queue* queue) {
    if (isEmpty(queue))
        return INT_MIN;
    return queue->array[queue->rear];
}

void print_queue(struct Queue* queue) {
	int i;
	printf("front ");
	for (i = 0; i < queue->size; ++i) {
		printf("%d ", queue->array[i]);
	}
	printf("end\n");
}
