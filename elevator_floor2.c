#include "elevator_floor.h"
#include "comm.h"

int floor_num = 1;

enum DIR dir = STOP;

char* dirmap[] = {"UP", "DOWN", "STOP", "OPENING_DOOR", "CLOSING_DOOR"};
int shmid;
int* shm_addr;

void alrm_func(int sn);
pthread_mutex_t *p_mutex_shared = NULL;



void* thread(void* arg)
{
    int cur_floor = -1, cur_dir = -1;
    pthread_detach(pthread_self());
    
    while(1){
        if(cur_floor != floor_num || cur_dir != dir){
            printf("---------------------------------------------------\n");
            printf("This is the sub thread of elevator floor 2\n");
            printf("The current floor is: %d\n", floor_num);
            printf("The current state of elevator is: %s\n",dirmap[dir]);
            printf("---------------------------------------------------\n\n");
        }
        cur_floor = floor_num;
        cur_dir = dir;
        sleep(0.5); 
    }

	pthread_exit(0);
}

int main(int argc, char** argv)
{
    shmid = GetShm(4096);
    shm_addr = shmat(shmid,0,0);
    printf("This is the main thread of elevator floor 2\n");

    //mutex 
    p_mutex_shared = (pthread_mutex_t *) shmat(shmid, NULL, 0) + 5;
	struct itimerval itimer = { {1, 0}, {1, 0} };
	setitimer(ITIMER_REAL, &itimer, NULL);
    signal(SIGALRM, alrm_func);


	pthread_t tid;
	int rc = pthread_create(&tid, NULL, thread, NULL);
    if(rc) {
        printf("Create pthread error!\n");
        return 1;
    }
    sleep(1);
    // pthread_join(tid,NULL);
    while(1) {
        printf("Please input the direction you want to go, u to go up, d to go down ,q to quit\n");
        char c = getchar();
        getchar();
        // take it for press a button in the controler
        if(c == 'u' || c == 'd') {
            //go up
            if (pthread_mutex_trylock(p_mutex_shared) == 0) {
                *(shm_addr + 3) = 1;
                printf("floor 3 change the shm\n");
                pthread_mutex_unlock(p_mutex_shared);
            }
        }
        else{
            printf(BLUE "pthread_mutex_trylock failed.\n" NONE);
        }
    }
    
    shmdt(shm_addr);

	exit(0);	
}


void alrm_func(int sn)
{
	if (p_mutex_shared == NULL)
	{
		printf("p_mutex_shared is NULL.\n");
		return ;
	} 
 
	time_t tm = time(NULL);
	printf("%s\t", ctime(&tm));
	if (pthread_mutex_trylock(p_mutex_shared) == 0)
	{
		printf(GREEN "pthread_mutex_trylock success.\n" NONE);

        floor_num = *shm_addr;
        dir = *(shm_addr+1);

        //printf(RED "after mutex floor is: %d, state is :%s\n" NONE,floor_num, dirmap[dir]);

		pthread_mutex_unlock(p_mutex_shared);
	}
	else
	{
		printf(BLUE "pthread_mutex_trylock failed.\n" NONE);
	}
	printf("################################################\n");
}