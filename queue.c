#include <semaphore.h>

#include "queue.h"
#include "sem.h"

// A structure to represent a queue
struct Queue {
    int front, rear, size, id;
    unsigned capacity;
    int* array;
    sem_t* mutex;
    sem_t* empty;
    sem_t* full;
};
 
// function to create a queue
// of given capacity.
// It initializes size of queue as 0
struct Queue* createQueue(unsigned capacity, int id)
{
    struct Queue* queue = (struct Queue*)malloc(sizeof(struct Queue));
    queue->capacity = capacity;
    queue->front = queue->size = 0;
 
    // This is important, see the enqueue
    queue->rear = capacity - 1;
    queue->array = (int*)malloc(queue->capacity * sizeof(int));

    queue->id = id;

    queue->mutex = make_sem(1);
    queue->empty = make_sem(capacity);
    queue->full = make_sem(0);

    return queue;
}
 
// Queue is full when size becomes
// equal to the capacity
int isFull(struct Queue* queue)
{
    return (queue->size == queue->capacity);
}
 
// Queue is empty when size is 0
int isEmpty(struct Queue* queue)
{
    return (queue->size == 0);
}
 
// Function to add an item to the queue.
// It changes rear and size
void enqueue(struct Queue* queue, int item)
{
    sem_wait(queue->empty);
    sem_wait(queue->mutex);

    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->array[queue->rear] = item;
    queue->size = queue->size + 1;

    sem_post(queue->mutex);
    sem_post(queue->full);
}
 
// Function to remove an item from queue.
// It changes front and size
int dequeue(struct Queue* queue)
{
    sem_wait(queue->full);
    sem_wait(queue->mutex);

    int item = queue->array[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size = queue->size - 1;
    
    sem_post(queue->mutex);
    sem_post(queue->empty);

    return item;
}
 
// Function to get front of queue
int front(struct Queue* queue)
{
    if (isEmpty(queue))
        return INT_MIN;
    return queue->array[queue->front];
}
 
// Function to get rear of queue
int rear(struct Queue* queue)
{
    if (isEmpty(queue))
        return INT_MIN;
    return queue->array[queue->rear];
}
 
