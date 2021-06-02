CC = gcc 


all: main

main: client server

client: client.c
	$(CC) client.c -o client.out

server: server.c
	$(CC) server.c -o server.out

clear: *.out