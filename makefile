CC = gcc
INC_DIR = lib
CFLAGS =-c -WALL -I
OBJ_DIR = object
liblru.so:lru.c
	$(CC) -g -fPIC -c lru.c
	$(CC) -shared lru.o -o liblru.so

test:test.c
	$(CC) -c test.c
	$(CC) -o test test.o -L. -llru
	
lru.o:lru.c
	$(CC) -c lru.c
dwf:dwf.c lru.o
	$(CC) -c -g dwf.c
	$(CC) -o dwf dwf.o lru.o -g

