CC=gcc
CFLAGS=-I.

parallel_sum: parallel_sum.o utils.o utils.h Summary.o
	$(CC) -o parallel_sum utils.o parallel_sum.o Summary.o  -lpthread  -L   $(CFLAGS)

parallel_sum.o: 
	$(CC) -c parallel_sum.c $(CFLAGS) $(CFLAGS)
utils.o : utils.h
	$(CC) -o utils.o -c utils.c $(CFLAGS)
Summary.o : Summary.h 
	$(CC) -fPIC -c Summary.c -o Summary.o	$(CFLAGS)

clean:
	rm *.o parallel_sum 