CC=gcc
CFLAGS=-I.

hello: main.c
	$(CC) -g -o hb main.c

install: hello
	install -m 744 hb /usr/local/bin
