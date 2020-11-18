CC = gcc
INC_DIR = lib
CFLAGS =-c -WALL -I
OBJ_DIR = object
lru:lru.c
	gcc -o lru lru.c

