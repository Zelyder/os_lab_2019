CC = gcc 


all: main

main: client server server2 client2

client: client.c
	$(CC) client.c -o client.out

server2: server2.c erproc.c
	$(CC) server2.c erproc.c -std=c99 -o server2.out

client2: client2.c erproc.c
	$(CC) client2.c erproc.c -std=c99 -o client2.out

server: server.c
	$(CC) server.c -o server.out

clear: *.out