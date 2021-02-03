#include "lib/lru.h"
#undef MAX_SIZE

int main(int argc, char* argv[]){

	int MAX_SIZE = atoi(argv[1]);
        FILE *f, *o;
        float time;
        int user, b_name, size, read;
	char buffer[100] = {0};
	sprintf(buffer, "data/%s", argv[2]);
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

	//Use index to represent the order
	int index = 0;
	int hit = 0;

	//Determine if over max size or not
	int p_count = 0;
	int d_count = 0;

	//tmprary carry index blockname from dram to pram
	int tmp_index;
	int tmp_b_name;
	int line = 0;
	

	//Deleted block name
	int d_b_name;
	
	hmap_init(&dram);
	hmap_init(&pram);
        while(fscanf(f, "%f %d %d %d %d", &time, &user, &b_name, &size, &read)== 5){
			

		//printf("line:%d ", line++);
		//printf("%d:",index);
		index++;
		if(block_find(b_name, dram) == NULL && block_find(b_name, pram) == NULL){
			//printf("miss ");
			if(read == 0){
				
				//DRAM is full
				if(d_count++ >= MAX_SIZE){
					//printf("d_over ");

					//If no node in PRAM
					//U can put node from DRAM to PRAM directly
					if(p_head== NULL){
						//printf("d->p ");
						//move from dram to pram
						tmp_b_name = d_tail->blockname;
						tmp_index = d_tail->index;
						d_b_name = block_delete(&d_tail);
						hmap_delete(d_b_name, dram);
						hmap_add(tmp_b_name, block_add(tmp_b_name, &p_head, &p_tail, tmp_index), pram);

					// Compare if invicted of DRAM is younger than head of PRAM
					// Yes -> put it to head of PRAM
					}else if(p_head != NULL && d_tail->index < p_head->index){
						//printf("d->p ");
						//move from dram to pram
						tmp_b_name = d_tail->blockname;
						tmp_index = d_tail->index;

						//If PRAM is full
						if(p_count >= MAX_SIZE){
							d_b_name = block_delete(&p_tail);
							hmap_delete(d_b_name, pram);
						}
						d_b_name = block_delete(&d_tail);
						hmap_delete(d_b_name, dram);
						hmap_add(tmp_b_name, block_add(tmp_b_name, &p_head, &p_tail, tmp_index), pram);
					//Otherwise just delete
					}else{
						d_b_name = block_delete(&d_tail);
						hmap_delete(d_b_name, dram);
					}
					d_count --;
				}
				hmap_add(b_name, block_add(b_name, &d_head, &d_tail, index++), dram);
			}
			//Put read in PRAM to protect PRAM
			else if(read == 1){
				if(p_count++ >= MAX_SIZE){
					d_b_name = block_delete(&p_tail);
					hmap_delete(d_b_name, pram);
					p_count --;
				}
				hmap_add(b_name, block_add(b_name, &p_head, &p_tail, index++), pram);
			}

		//Cache hit
		}else{
			hit++;
			if(block_find(b_name, dram) != NULL){
			       block_alter(block_find(b_name, dram), &d_head, &d_tail);
			}else if(block_find(b_name, pram) != NULL){
			       block_alter(block_find(b_name, pram), &p_head, &p_tail);
			}
		}

        }
	

	fprintf(o, "%d %f\n",MAX_SIZE, (float)hit/index);
	fclose(o);
	fclose(f);
        return 0;


}
