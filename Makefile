CC=gcc
CFLAGS= 

main: main.o queue.o
	$(CC) -o main main.o queue.o -lrt -pthread

main2: main2.o queue.o
	$(CC) -o main2 main2.o queue.o -lrt -pthread

clean:
	rm -f main main2 main.o main2.o queue.o
