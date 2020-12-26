CC = gcc
INC_DIR = lib
CFLAGS =-c -WALL -I
OBJ_DIR = object
liblru.so:lru.c
	$(CC) -g -fPIC -c lru.c
	$(CC) -shared lru.o -o liblru.so
	rm lru.o

