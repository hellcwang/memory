/*****************************
 * This program implement lru algorithm
 * Use hashmap + double linked list to inplement
 *
 * hellc
 * 28102020
 *****************************/
#include <stdio.h>
#include <stdlib.h>

/**********************
 * MAX_SIZE is the max size of memory
 * Can alternative eazy
 *********************/
#define MAX_SIZE 500

/***************************
 * key = memory block name
 * value = address of key in linked list
 * next point to next struct of map
 *****************************/
typedef struct Hash{
        int key;
        void* value;
        struct Hash *next;
}map;

/*****************************
 * Node of "double" linked list
 * Main value is blockname
 * Fast to mantain
 ****************************/
typedef struct Node{
        struct Node *pre;
        int blockname;
        struct Node *next;
}node;

void block_find(int);
node* block_delete(void);
void block_alter(node*);
node* block_add(int);
void hmap_delete(int);
void hmap_add(int, void*);


