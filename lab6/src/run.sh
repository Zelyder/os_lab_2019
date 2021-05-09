#!/bin/sh
make client && make server
./server.out --port 8888 --tnum 2 &
./server.out --port 8889 --tnum 3 &
./client.out --k 20 --mod 1234 --servers servers.txt &