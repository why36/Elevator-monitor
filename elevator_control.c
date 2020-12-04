#include "elevator_floor.h"
#include "comm.h"

int floor_num = 1;

enum DIR dir = STOP;

char* dirmap[] = {"UP", "DOWN", "STOP", "OPENING_DOOR", "CLOSING_DOOR"};
int shmid;
int* shm_addr;
pthread_mutex_t *p_mutex_shared = NULL;
void alrm_func(int sn);

void move(int);
void* thread(void* arg)
{
    int cur_floor = -1, cur_dir = -1;
    pthread_detach(pthread_self());
    while(1){
        if(cur_floor!=floor_num || cur_dir != dir){
            printf("---------------------------------------------------\n");
            printf("This is the sub thread of elevator controler\n");
            printf("The current floor is: %d\n", floor_num);
            printf("The current state of elevator is: %s\n",dirmap[dir]);
            printf("---------------------------------------------------\n\n");
        }
        cur_floor = floor_num;
        cur_dir = dir;
    }

	pthread_exit(0);
}

int main(int argc, char** argv)
{
    shmid = CreateShm(4096);
    shm_addr = shmat(shmid,0,0);
    *shm_addr = floor_num;
    *(shm_addr + 1) = dir;
    *(shm_addr + 2) = 0;*(shm_addr + 3) = 0;*(shm_addr + 4) = 0;

    // shared mutex
    p_mutex_shared = (pthread_mutex_t*) shm_addr + 5 ;
	pthread_mutexattr_t mutextattr;
	pthread_mutexattr_init(&mutextattr);
	pthread_mutexattr_setpshared(&mutextattr, PTHREAD_PROCESS_SHARED);
	pthread_mutex_init(p_mutex_shared, &mutextattr);

    signal(SIGALRM, alrm_func);
	//signal(SIGINT, int_func);
 
	//struct itimerval itimer = { {0, 100000}, {0, 100000} };
    struct itimerval itimer = { {1, 0}, {1, 0} };
	setitimer(ITIMER_REAL, &itimer, NULL);



    printf("This is the main thread of elevator controler\n");
	pthread_t tid;
	int rc = pthread_create(&tid, NULL, thread, NULL);
    if(rc) {
        printf("Create pthread error!\n");
        return 1;
    }
    sleep(1);
    // pthread_join(tid,NULL);
    while(1) {
        printf("Please input the floor you want to go, o to open the door, c to close the door,q to quit\n");
        char c = getchar();
        getchar();
        if(c == '1') {
            move(1);
        }
        if(c == '2') {
            move(2);
        }
        if(c == '3') {
            move(3);
        }
        if(c == 'o') {
            dir = OPENING_DOOR;
        }
        if(c == 'c') {
            dir = CLOSING_DOOR;
        }
        if(c == 'q') break;

        *shm_addr = floor_num;
        *(shm_addr + 1) = dir;
    }
    
    shmdt(shm_addr);
    DestroyShm(shmid);

	exit(0);	
}

void move(int f) {
    printf("go to floor %d\n", f);
    sleep(1);
    dir = CLOSING_DOOR;
    sleep(0.5);
    if(floor_num > f) dir = DOWN;
    else if(floor_num < f) dir = UP;
    else return;
    sleep(1);
    floor_num = f;
    dir = OPENING_DOOR;
    sleep(0.5);
    dir = CLOSING_DOOR;
    sleep(0.5);
    dir = STOP;
    return;
}

void alrm_func(int sn)
{
	if (p_mutex_shared == NULL)
	{
		printf("p_mutex_shared is NULL.\n");
		return ;
	}
 
	time_t tm = time(NULL);
	//printf("%s\t", ctime(&tm));
	if (pthread_mutex_trylock(p_mutex_shared) == 0)
	{
		//printf(GREEN "pthread_mutex_trylock success.\n" NONE);

        floor_num = *shm_addr;
        dir = *(shm_addr+1);
        for(int i = 2; i < 5;++i) {
            if(*(shm_addr+i) == 1) {
                move(i-1);
                *shm_addr = floor_num;
                *(shm_addr + 1) = dir;
                *(shm_addr+i) = 0;
            }
        }
        //printf(RED "after mutex floor is: %d, state is :%s\n" NONE,floor_num, dirmap[dir]);

		pthread_mutex_unlock(p_mutex_shared);
	}
	else
	{
		printf(BLUE "pthread_mutex_trylock failed.\n" NONE);
	}
	//printf("################################################\n");
}