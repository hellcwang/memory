CC = gcc
INC_DIR = lib
CFLAGS =-c -WALL -I
OBJ_DIR = object
liblru.so:lru.c
	$(CC) -g -fPIC -c lru.c
	$(CC) -shared lru.o -o liblru.so
<<<<<<< HEAD

test:test.c
	$(CC) -c test.c
	$(CC) -o test test.o -L. -llru
	
lru.o:lru.c
	$(CC) -c lru.c
dwf:dwf.c lru.o
	$(CC) -c -g dwf.c
	$(CC) -o dwf dwf.o lru.o -g
=======

lru.o:lru.c lib/lru.h
	$(CC) -c lru.c

test:test.c
	$(CC) -c test.c
	$(CC) -o test test.o lru.o

dwf:dwf.c lru.o
	$(CC) -c dwf.c
	$(CC) -o dwf dwf.o lru.o
>>>>>>> 294ef7b76c34e3f3f746d0afa6f5bde295e26188

