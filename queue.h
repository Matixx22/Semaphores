#ifndef QUEUE_H
#define QUEUE_H

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct Queue Queue;

struct Queue* createQueue(unsigned capacity, int id);
int isFull(struct Queue* queue);
int isEmpty(struct Queue* queue);
void enqueue(struct Queue* queue, int item, int prod_id);
int dequeue(struct Queue* queue, int cons_id);
int front(struct Queue* queue);
int rear(struct Queue* queue);

#endif /* QUEUE_H */
