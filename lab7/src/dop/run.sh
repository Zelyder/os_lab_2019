#!/bin/sh
make

./.out --buf_size 100 --port 8889 &
./udpclres.out --buf_size 100 --ip 127.0.0.1 --port 8889