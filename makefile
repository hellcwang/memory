CC = gcc
INC_DIR = lib
CFLAGS =-c -WALL -I
OBJ_DIR = object
liblru.so:lru.c
	$(CC) -g -fPIC -c lru.c
	$(CC) -shared lru.o -o liblru.so

lru.o:lru.c lib/lru.h
	$(CC) -c lru.c

test:test.c
	$(CC) -c test.c
	$(CC) -o test test.o lru.o

dwf:dwf.c lru.o
	$(CC) -c dwf.c
	$(CC) -o dwf dwf.o lru.o

