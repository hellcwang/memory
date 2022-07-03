CC = gcc
INC_DIR = lib
CFLAGS =-c -WALL -I
OBJ_DIR = object
OBJ_FLAGS = -c
DEBUG = -g

all:dwf cache total_lru multi

liblru.so:lru.c
	$(CC) $(DEBUG) -fPIC $(OBJ_FLAGS) $<
	$(CC) -shared lru.o -o $@ 

test:test.c
	$(CC) $(OBJ_FLAGS) $< 
	$(CC) -o $@ test.o -L. -llru
	
lru.o:lru.c
	$(CC) $(OBJ_FLAGS) $<
dwf:dwf.c lru.o
	$(CC) $(OBJ_FLAGS) $(DEBUG) dwf.c
	$(CC) -o $@ dwf.o lru.o $(DEBUG)

cache:dram_cache.c lru.o
	$(CC) $(OBJ_FLAGS) $(DEBUG) dram_cache.c
	$(CC) -o cache dram_cache.o lru.o $(DEBUG)

total_lru:total_lru.c lru.o
	$(CC) $(OBJ_FLAGS) $(DEBUG) total_lru.c
	$(CC) -o total_lru total_lru.o lru.o $(DEBUG)
	rm total_lru.o

multi:multiusr.c lru.o
	$(CC) $(OBJ_FLAGS) $(DEBUG)  multiusr.c 
	$(CC) -o multi   multiusr.o lru.o $(DEBUG) 
	rm multiusr.o

.PHONY:clean

clean:
	rm dwf multi total_lru cache *.o
