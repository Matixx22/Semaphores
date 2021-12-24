CC=gcc
CFLAGS= 

main: main.o sem.o queue.o
	$(CC) -o program main.o sem.o queue.o -lrt -pthread

clean:
	rm -f program main.o queue.o sem.o
