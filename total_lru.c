#include "lib/lru.h"
#undef MAX_SIZE


int main(int argc, char* argv[]){

	int MAX_SIZE = atoi(argv[1])*2;
        FILE *f, *o;
        float time;
        int user, b_name, size, read;
	char buffer[100] = {0};
	sprintf(buffer, "total_lru_data/%s", argv[2]);
        f = fopen(buffer, "r");
	if(f == NULL){
		printf("f ERROR\n");
		return 1;
	}
	sprintf(buffer, "%s.log", buffer);
	o = fopen(buffer,"a");
	if(o == NULL){
		printf("o ERROR\n");
		return 1;
	}
        if(f == NULL){
                printf("NO FILE!!\n");
                return -1;
        }

	//Use to map for to type of memory
	//Need to indepdent linked list
	//So there is too head and tail
	map **dram, **pram;
        node *d_head = NULL;
        node *p_head = NULL;
        node *d_tail = NULL;
        node *p_tail = NULL;

	node* locate;

	//Use index to represent the order
	int index = 0;
	int hit = 0;

	//write count on pram
	int write_p = 0;

	//Determine if over max size or not
	int p_count = 0;
	int d_count = 0;

	//tmprary carry index blockname from dram to pram
	int tmp_index;
	int tmp_index_1;
	int tmp_b_name_1;
	int tmp_b_name;
	int line = 0;
	

	//Deleted block name
	int d_b_name;

	//count of dram is youmger than pram
	int y_count = 0;

	int type_count = 0;
	
	hmap_init(&dram);
        while(fscanf(f, "%f %d %d %d %d", &time, &user, &b_name, &size, &read)== 5){
			

		//printf("line:%d ", line++);
		//printf("%d:",index);
		index++;
		if(block_find(b_name, dram) == NULL ){
				if(d_count >= MAX_SIZE/2 && type_count <= MAX_SIZE/2){  //half part of lru is pram
					locate = d_tail;
					while(locate->type == 1)
						locate = locate->pre;
					locate->type = 1;
                                        write_p ++;                                     //write on pram
					type_count ++;
				}

				if(d_count++ >= MAX_SIZE){
					d_b_name = block_delete(&d_tail);
					hmap_delete(d_b_name, dram);
					d_count --;
				}
				hmap_add(b_name, block_add(b_name, &d_head, &d_tail, index++), dram);
			

		//Cache hit
		}else{
			hit++;
			if(block_find(b_name, dram) != NULL){
				if(block_find(b_name, dram)->type == 1 && read == 0)
				       write_p ++;	
			       	block_alter(locate = block_find(b_name, dram), &d_head, &d_tail);
				if(locate->type == 1){
			       		locate->type = 0;
					locate = d_tail;
					while(locate->type == 1)
						locate = locate->pre;
					locate->type = 1;
				}
			}
		}

        }
	

	fprintf(o, "%4d\t%5f\t%5f\n",MAX_SIZE/2, (float)hit/index, (float)write_p/index);
	//printf("%4d\t%5f\t%5f\n",MAX_SIZE/2, (float)hit/index, (float)write_p/index);
	//printf("ycount:%d %f%% \n",y_count, (float)y_count/index);
	fclose(o);
	fclose(f);
        return 0;


}
