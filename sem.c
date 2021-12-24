#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

#include "sem.h"

void *check_malloc(int size) {
        void *p = malloc(size);
        if (p == NULL) {
		perror("malloc failed");
		exit(-1);
	}
        return p;
}

sem_t* make_sem(int value) {
        sem_t* sem = check_malloc(sizeof(sem_t));
        int n = sem_init(sem, 0, value);
        if (n != 0) {
		perror("sem_init failed");
		exit(-1);
	}
        return sem;
}
