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

cache:dram_cache.c lru.o
	$(CC) -c -g dram_cache.c
	$(CC) -o cache dram_cache.o lru.o -g

total_lru:total_lru.c lru.o
	$(CC) -c -g total_lru.c
	$(CC) -o total_lru total_lru.o lru.o -g
	rm total_lru.o

multi:multiusr.c lru.o
	$(CC) -c -g  multiusr.c 
	$(CC) -o multi   multiusr.o lru.o -g 
	rm multiusr.o
