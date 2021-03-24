/*****************************
 * This program implement lru algorithm
 * Use hashmap + double linked list to inplement
 *
 * hellc
 * 28102020
 *****************************/
#include "./lib/lru.h"

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
/*typedef struct Hash{
	int key;
	void* value;
	struct Hash *next;
}map;
*/

/*****************************
 * Node of "double" linked list
 * Main value is blockname
 * Fast to mantain
 ****************************/
/*typedef struct Node{
	struct Node *pre;
	int index;
	int blockname;
	struct Node *next;
}node;
*/

/*
 * Use *** to sent pointer of map back to main
 * So the hash map is array of pointer to each entry
 **/
void hmap_init(map ***first){
	int i, j;
	map* tmp;
	*first = malloc(MAX_SIZE * sizeof(map*));
	for(i = 0;i < MAX_SIZE;i ++){
		tmp = *((*first )+i)= malloc(sizeof(map));
		tmp->value = NULL;
		tmp->key = -1;
		tmp->next = NULL;
	}
	return;
}

void hmap_add(int blockname, node* n, map** h){
	map *current;
	int key = blockname % MAX_SIZE;
	current = h[key];

	while(current->value != NULL){
		current = current->next;
	}

	current->value = n;
	current->key = blockname;
	current->next = malloc(sizeof(map));
	current = current->next;
	current->value = NULL;
	current->next = NULL;

	return ;
}

void hmap_delete(int blockname, map** h){
	int key = blockname % MAX_SIZE;
	map* current = h[key];
	map* pre = NULL;

	while(current->key != blockname){
		pre = current;
		current = current->next;
	}

	if(pre != NULL){
		pre->next = current->next;
		free(current);
		return ;
	}else{
		if(current->next != NULL){
			h[key] = current->next;
			free(current);
		}else{
		current->value = current->next = NULL;
		current->key = -2;
		}
		return ;
	}
}

node* block_add(int b_name, node** head, node** tail,int index){
	node* new;
	new = malloc(sizeof(node));
	if(*tail == NULL)
		*tail = new;
	if(*head != NULL){
		(*head)->pre = new;
		new->next = *head;
		new->pre = NULL;
		*head = new;
	}else{
		new->next = NULL;
		new->pre = NULL;
		*head = new;
	}
	new->index  = index;
	new->blockname = b_name;	
	new->type = 0; //default is dram(0),pram is (1)
	
	return new;
}

void block_alter(node* n, node** head, node** tail){
	if(n == *head)
		return ;
	if(n == *tail ){
		*tail = n->pre;
	}
	if(n != *head){
		n->pre->next = n->next;
		if(n->next != NULL)
			n->next->pre = n->pre;
		n->pre = NULL;
		n->next = *head;
		(*head)->pre = n;
	}
	*head = n;
	return ;
}

int block_delete(node** tail){
	int b_name = (*tail)->blockname;
	(*tail)->pre->next = NULL;
	node* d = *tail;
	*tail = (*tail)->pre;
	free(d);
	return b_name;
}
	

node* block_find(int b_name, map** h){
	int key = b_name % MAX_SIZE;
	map* cur = h[key];

	while(cur->key != b_name && cur->next != NULL){
		cur = cur->next;
	}
	if(cur->next == NULL)
		return NULL;
	return cur->value;
}


