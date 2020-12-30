#include "lib/lru.h"

int main(void){

        FILE *f;
        float time;
        int user, b_name, size, read;
        f = fopen("run1_usr1.txt", "r");
	//f = fopen("same","r");
        if(f == NULL){
                printf("NO FILE!!\n");
                return -1;
        }

	map **dram, **pram;
        node *d_head = NULL;
        node *p_head = NULL;
        node *d_tail = NULL;
        node *p_tail = NULL;
	int index = 0;
	int p_count = 0;
	int d_count = 0;
	int tmp_index;
	int tmp_b_name;
	int line = 0;
	int d_b_name;
	
	hmap_init(&dram);
	hmap_init(&pram);
        while(fscanf(f, "%f %d %d %d %d", &time, &user, &b_name, &size, &read)== 5){
			

		//printf("line:%d ", line++);
		//printf("%d:",index);
		if(block_find(b_name, dram) == NULL && block_find(b_name, pram) == NULL){
			//printf("miss ");
			if(read == 0){
				if(d_count++ >= MAX_SIZE){
					//printf("d_over ");

					if(p_head== NULL){
						//printf("d->p ");
						//move from dram to pram
						tmp_b_name = d_tail->blockname;
						tmp_index = d_tail->index;
						d_b_name = block_delete(&d_tail);
						hmap_delete(d_b_name, dram);
						hmap_add(tmp_b_name, block_add(tmp_b_name, &p_head, &p_tail, tmp_index), pram);
					}else if(p_head != NULL && d_tail->index < p_head->index){
						//printf("d->p ");
						//move from dram to pram
						tmp_b_name = d_tail->blockname;
						tmp_index = d_tail->index;
						if(p_count >= MAX_SIZE){
							d_b_name = block_delete(&p_tail);
							hmap_delete(d_b_name, pram);
						}
						d_b_name = block_delete(&d_tail);
						hmap_delete(d_b_name, dram);
						hmap_add(tmp_b_name, block_add(tmp_b_name, &p_head, &p_tail, tmp_index), pram);
					}else{
						d_b_name = block_delete(&d_tail);
						hmap_delete(d_b_name, dram);
					}
					d_count --;
				}
				hmap_add(b_name, block_add(b_name, &d_head, &d_tail, index++), dram);
			}
			else if(read == 1){
				if(p_count++ >= MAX_SIZE){
					d_b_name = block_delete(&p_tail);
					hmap_delete(d_b_name, pram);
					p_count --;
				}
				hmap_add(b_name, block_add(b_name, &p_head, &p_tail, index++), pram);
			}
		}else{
			//printf("hit ");
			if(block_find(b_name, dram) != NULL){
			       block_alter(block_find(b_name, dram), &d_head, &d_tail);
			}else if(block_find(b_name, pram) != NULL){
			       block_alter(block_find(b_name, pram), &p_head, &p_tail);
			}
		}

        }
	

	fclose(f);
        return 0;


}
