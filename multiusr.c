#include "lib/lru.h"
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/msg.h>
#include <fcntl.h>
#include <string.h>
//pid for fork()
pid_t pid;

struct M{
	long mtype;
	float carry[4];
}m_buffer;
int msqid;
key_t key;
#define PERMS 0644


//Determine if over max size or not
int p_count = 0;
int d_count = 0;

//write count on pram
int write_p = 0;



//transfer data
int delta;
float slope_child;
float slope;

FILE *f, *o;

int k = 0;
int h = 100;
//Use these variable to mark blocks
int MAX_VALUE;
int O_VALUE;
int GHOST = 3;		//Can change to find best solution.


//Use myindex to represent the order
float old_hit = 0.0;
float old_hit_ghost = 0.0;	//hit on ghost
float old_hit_o= 0.0;		//hit on original size

//Use myindex to represent the order
int myindex = 0;	//index is conflict to finction been declared
int hit = 0;
int hit_ghost = 0;	//hit on ghost
int hit_o= 0;		//hit on original size
//if one process done first done = -1
int done = 0;

/*Change the size of each process
 */
void handler(int sig_num){
	printf("%d alarm pid:%d\n", getpid(), pid);
	if(pid != 0){
		kill(pid,SIGALRM);
	}
	if(done == -1){
		MAX_VALUE = O_VALUE;
		printf("-1 %d alarm\n", getpid());
		fprintf(o, "%4d\t%5f\t%5f\n",MAX_VALUE, (float)hit/myindex, (float)write_p/myindex);
		return ;
	}
	signal(SIGALRM, handler);
	//next hit (predict)
	float n_hit = 0;
	float n_hit_ghost = 0;
	float n_hit_o = 0;
	float c_hit = 0;
	float c_hit_ghost = 0;
	float c_hit_o = 0;
	float percent = 0.0;
	float percent_child = 0.0;
	float slope_child = 0.0;
	int i = 0;
	int change = 0;

	if(old_hit == 0.0){
		printf("in\n");
		old_hit = (float)hit / myindex;
		old_hit_ghost = (float)hit_ghost / myindex;
		old_hit_o = (float)hit_o / myindex;
	//fprintf(o, "n_hit:%f o_hit:%f ",n_hit, old_hit);
	}
	printf("old_hit_o:%f\n", old_hit_o);
	printf("hit_o:%d\n", hit_o);
	printf("myindex:%d\n", myindex);

	c_hit = (float)hit_ghost / myindex;
	c_hit_ghost = (float)hit_ghost / myindex;
	c_hit_o = (float)hit_o / myindex;

	//fprintf(o, "n_hit:%f o_hit:%f ",n_hit, old_hit);
	n_hit = 0.7 * old_hit + 0.3 * c_hit;
	n_hit_ghost = 0.7 * old_hit_ghost + 0.3 * c_hit_ghost;
	n_hit_o = 0.7 * old_hit_o + 0.3 * c_hit_o;
	printf("n_hit: %f\n",n_hit);
	percent = n_hit/n_hit_o;
	fprintf(o, "n_hit:%f n_hit_o:%f\n", n_hit, n_hit_o);


	//fprintf(o, "n_hit:%f o_hit:%f ",n_hit, old_hit);
	
	//initial m_buffer
	m_buffer.carry[0] = 0.0;
	m_buffer.carry[1] = 0.0;
	m_buffer.carry[2] = 0.0;
	m_buffer.carry[3] = 0.0;
	//msg sent to parrent
	if(pid == 0){
		m_buffer.carry[0] = (double)percent;
		m_buffer.carry[1] = (double)n_hit_ghost/n_hit_o;
		m_buffer.carry[2] = (double)MAX_VALUE;
		//fprintf(o,"child %lf %lf %d\n",m_buffer.carry[0],m_buffer.carry[1], (int)m_buffer.carry[2]);
		m_buffer.mtype = 1;	//child to parent
		msgsnd(msqid, &m_buffer, 32, 0);
		fprintf(o,"child s%f %f %d\n",m_buffer.carry[0],m_buffer.carry[1], (int)m_buffer.carry[2]);
		if(msgrcv(msqid, &m_buffer, 32, 0, 0) == -1){
			perror("msgrcv");
			exit(1);
		}
		change = (int)m_buffer.carry[0];
		fprintf(o,"childr %d\n",(int)m_buffer.carry[0]);
	}
	//parent rcv
	if(pid > 0){
		if(msgrcv(msqid, &m_buffer, 32, 0, 0) == -1){
			perror("msgrcv");
			exit(1);
		}
		if((int)m_buffer.carry[3] == -1){
			done = -1;
			MAX_VALUE = O_VALUE;
			printf("-1 %d alarm\n", getpid());
			fprintf(o, "%4d\t%5f\t%5f\n",MAX_VALUE, (float)hit/myindex, (float)write_p/myindex);
			return ;
		}
		
		fprintf(o,"parent r%lf %lf %d\n",m_buffer.carry[0],m_buffer.carry[1], (int)m_buffer.carry[2]);
		float diff, c_percent;
		int c_size = m_buffer.carry[2];
		c_percent = (float)m_buffer.carry[0];
		diff = percent - (float)m_buffer.carry[0];
		if(c_percent < percent){
		while(diff > 0){
			if(c_percent < 0.5){
				change = (0.5-c_percent) * O_VALUE;
				if(change > (MAX_VALUE - 4)){
					change = MAX_VALUE - 4;
					break;
					
				}
			}
			change ++;
			c_percent  = c_percent *(c_size+change)/c_size;
			diff = percent - c_percent;
		}
		}
		else if(c_percent > percent){
			diff = c_percent - percent;
		while(diff > 0){
			if(percent < 0.5){
				change = (0.5 - percent) * O_VALUE;
			}
			change ++;
			if(change > (c_size-4)){
				change = c_size - 4;
				break;
			}
			percent  = percent *(MAX_VALUE +change)/MAX_VALUE;
			diff = c_percent - percent;
		}
		change = 0-change;
		}


		m_buffer.carry[0] = (double)change;
		m_buffer.mtype = 2;	//child to parent
		msgsnd(msqid, &m_buffer, 8, 0);
		fprintf(o,"parent s %d \n",(int)m_buffer.carry[0]);
	}

	
	
	fprintf(o, "%4d\t%5f\t%5fpercent:%5f\n",MAX_VALUE, (float)hit/myindex, (float)write_p/myindex, percent);
	hit = 0;
	hit_o = 0;
	hit_ghost = 0;
	myindex = 0;
	//MAX_VALUE --;
	old_hit = n_hit;
	old_hit_ghost =  n_hit_ghost;
	old_hit_o = n_hit_o;
	if(pid == 0){
		MAX_VALUE += change;
	}else if(pid > 0){
		MAX_VALUE -= change;
	}
	return ;
}



int main(int argc, char* argv[]){
	signal(SIGALRM, handler);
	

	//fork() initial
	pid = fork();
	

	//Use to map for to type of memory
	//Need to indepdent linked list
	//So there is too head and tail
	map **dram, **pram;
        node *d_head = NULL;
        node *p_head = NULL;
        node *d_tail = NULL;
        node *p_tail = NULL;

	node* locate, locate_1;




	//tmprary carry myindex blockname from dram to pram
	int tmp_index;
	int tmp_b_name;
	int line = 0;
	

	//Deleted block name
	int d_b_name;

	//count of dram is younger than pram
	int y_count = 0;
	
	hmap_init(&dram);
	hmap_init(&pram);


	O_VALUE = atoi(argv[1]);
	MAX_VALUE = atoi(argv[1])/2;
        float time;
        int user, b_name, size, read;
	char buffer[100] = {0};
	int i, j;
	node *tmp, *find;	//tmp is for MARK, find for 




	//msq initial
	if((key = ftok(".",99)) == -1){
		perror("ftok");
		exit(1);
	}
	if((msqid = msgget(key, PERMS | IPC_CREAT)) == -1){
		perror("msgget");
		exit(1);
	}
	/*process 0*/
	if(pid > 0){
	sprintf(buffer, "mul/%s", argv[2]);
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
	}
	/*process 1*/
	if(pid == 0){
	sprintf(buffer, "mul/%s", argv[3]);
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
	}



	fprintf(o, "Begin:\n");
	if(pid > 0)
		ualarm(100000, 100000);
        while(fscanf(f, "%f %d %d %d %d", &time, &user, &b_name, &size, &read)== 5){
			

		//for(int i = 0;i < 1000;i ++);
		//printf("line:%d ", line++);
		//printf("%d:%d ",getpid(), myindex);
		myindex ++;

		
		if(block_find(b_name, dram) == NULL && block_find(b_name, pram) == NULL){
			//printf("miss ");
			if(read == 0){
				
				//DRAM is full
				d_count ++;
				while(d_count > O_VALUE){
					//printf("d_over ");

					//If no node in PRAM
					//U can put node from DRAM to PRAM directly
					if(p_head== NULL){
					        write_p ++;
						//printf("d->p ");
						//move from dram to pram
						tmp_b_name = d_tail->blockname;
						tmp_index = d_tail->index;
						d_b_name = block_delete(&d_tail);
						hmap_delete(d_b_name, dram);
						hmap_add(tmp_b_name, block_add(tmp_b_name, &p_head, &p_tail, tmp_index), pram);

					// Compare if invicted of DRAM is younger than head of PRAM
					// Yes -> put it to head of PRAM
					}else if(p_tail != NULL && d_tail->index > p_tail->index){
						p_count ++;
					        write_p ++;
						y_count++;
						//printf("d->p ");
						//move from dram to pram
						tmp_b_name = d_tail->blockname;
						tmp_index = d_tail->index;

						//If PRAM is full
						while(p_count > O_VALUE){
							d_b_name = block_delete(&p_tail);
							hmap_delete(d_b_name, pram);
							p_count --;
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
				hmap_add(b_name, block_add(b_name, &d_head, &d_tail, myindex++), dram);
			}
			//Put read in PRAM to protect PRAM
			else if(read == 1){
				p_count ++;
				while(p_count > O_VALUE){
					d_b_name = block_delete(&p_tail);
					hmap_delete(d_b_name, pram);
					p_count --;
				}
				write_p ++;
				hmap_add(b_name, block_add(b_name, &p_head, &p_tail, myindex++), pram);
			}

		//Cache hit
		}else{
			hit_o ++;
			if(block_find(b_name, dram) != NULL){

				//caculate marks
				find = block_find(b_name, dram);
				if(find->ghost ==0 && find->o_size == 0){
					hit_ghost ++;
					hit ++;
				}else if(find ->ghost){
					hit_ghost ++;
				}

				block_alter(block_find(b_name, dram), &d_head, &d_tail);

			}else if(block_find(b_name, pram) != NULL){
				//caculate marks
				find = block_find(b_name, pram);
				if(find->ghost ==0 && find->o_size == 0){
					hit_ghost ++;
					hit ++;
				}else if(find ->ghost){
					hit_ghost ++;
				}

				if(read == 0){
					write_p ++;
					locate = block_find(b_name, pram);
					tmp_b_name = locate->blockname;
					tmp_index = locate->index;
					hmap_delete(tmp_b_name, pram);
					hmap_add(d_tail->blockname, locate, pram);
					
					locate->blockname = d_tail->blockname;
					locate->index = d_tail->index;
					//////////pram done/////////
					/////////dram /////////

					hmap_delete(d_tail->blockname, dram);
					hmap_add(tmp_b_name, d_tail, dram);
					
					d_tail->blockname = tmp_b_name;
					d_tail->index = tmp_index;
						
			       		block_alter(d_tail, &d_head, &d_tail);



				}else{
			       		block_alter(block_find(b_name, pram), &p_head, &p_tail);
				}
			}
		}

		//Mark
		if(d_count > MAX_VALUE){
			tmp = d_tail;
			if(d_count >(MAX_VALUE + GHOST)){
				for(i = d_count - (MAX_VALUE + GHOST);i > 0;i --){
					tmp->o_size = 1;
					tmp = tmp->pre;
				}
				for(i = GHOST;i > 0;i --){
					tmp->ghost = 1;
					tmp->o_size = 0;
					tmp = tmp->pre;
				}
			}else if(d_count > MAX_VALUE){
				for(i = d_count - MAX_VALUE;i > 0;i --){
					tmp->ghost = 1;
				}
			}
			tmp = tmp->pre;
			while(tmp != NULL){
				tmp->ghost = 0;
				tmp->o_size = 0;
				tmp = tmp->pre;
			}

		}
		if(p_count > MAX_VALUE){
			tmp = p_tail;
			if(p_count >(MAX_VALUE + GHOST)){
				for(i = p_count - (MAX_VALUE + GHOST);i > 0;i --){
					tmp->o_size = 1;
					tmp = tmp->pre;
				}
				for(i = GHOST;i > 0;i --){
					tmp->ghost = 1;
					tmp = tmp->pre;
				}
			}else if(p_count > MAX_VALUE){
				for(i = p_count - MAX_VALUE;i > 0;i --){
					tmp->ghost = 1;
				}
			}
			tmp = tmp->pre;
			while(tmp != NULL){
				tmp->ghost = 0;
				tmp->o_size = 0;
				tmp = tmp->pre;
			}
		}
		tmp = NULL;
	
        }
	

	fprintf(o, "total:%4d\t%5f\t%5f\n\n",MAX_VALUE, (float)hit/myindex, (float)write_p/myindex);
	//fprintf(stdout, "%4d\t%5f\t%5f\n",MAX_VALUE, (float)hit/myindex, (float)write_p/myindex);
	//printf("ycount:%d %f%% \n",y_count, (float)y_count/myindex);
	fclose(o);
	fclose(f);
	alarm(0);
	done = -1;	//process done 
	//child done first
	if(pid == 0){
		m_buffer.carry[3] = (double)-1;
		m_buffer.mtype = 1;
		msgsnd(msqid, &m_buffer, 32, 0);
	}

	printf("done\n");
	int exit_status;
	if(pid > 0)
		wait(&exit_status);
	//msg Q close
	if(pid > 0)
	if (msgctl (msqid, IPC_RMID, NULL) == -1) {
            	perror ("client: msgctl");
            	exit (1);
    	}
        return 0;


}
