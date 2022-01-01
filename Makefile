CC=gcc
CFLAGS= 

main: main.o sem.o queue.o
	$(CC) -o program main.o sem.o queue.o -lrt -pthread

main-upgrade: main-upgrade.o sem.o queue.o
	$(CC) -o program-upgrade main-upgrade.o sem.o queue.o -lrt -pthread
clean:
	rm -f program program-upgrade main.o main-upgrade.o queue.o sem.o
