CC=gcc
CFLAGS=-I.

all : parallel_min_max zombie process_memory

process_memory: process_memory.c
	$(CC) -o process_memory process_memory.c $(CFLAGS)

zombie : zombie.c 
	$(CC) -o zombie zombie.c $(CFLAGS)

parallel_min_max : utils.o find_min_max.o utils.h find_min_max.h parallel_min_max.c
	$(CC) -o parallel_min_max utils.o find_min_max.o parallel_min_max.c $(CFLAGS)
  

utils.o : utils.h utils.c
	$(CC) -o utils.o -c utils.c $(CFLAGS)

find_min_max.o : utils.h find_min_max.h find_min_max.c
	$(CC) -o find_min_max.o -c find_min_max.c $(CFLAGS)


clean :
	rm -rf *.o sequential_min_max parallel_min_max zombie process_memory