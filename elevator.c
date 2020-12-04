#include "elevator_floor.h"
#include "comm.h"

int floor_num = 1;

enum DIR dir = STOP;

char* dirmap[] = {"UP", "DOWN", "STOP", "OPENING_DOOR", "CLOSING_DOOR"};
int shmid;
int* shm_addr;

void alrm_func(int sn);
pthread_mutex_t *p_mutex_shared = NULL;


int main(int argc, char** argv)
{
    shmid = GetShm(4096);
    shm_addr = shmat(shmid,0,0);
    printf("This is the main thread of elevator \n");

    //mutex 
    p_mutex_shared = (pthread_mutex_t *) shmat(shmid, NULL, 0) + 5;
	struct itimerval itimer = { {1, 0}, {1, 0} };
	setitimer(ITIMER_REAL, &itimer, NULL);
    signal(SIGALRM, alrm_func);

    sleep(1);
    // pthread_join(tid,NULL);
    while(1) {
        ;
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

        printf(RED "after mutex floor is: %d, state is :%s\n" NONE,floor_num, dirmap[dir]);

		pthread_mutex_unlock(p_mutex_shared);
	}
	else
	{
		printf(BLUE "pthread_mutex_trylock failed.\n" NONE);
	}
	printf("################################################\n");
}