/*  Jordan Sinoway
    CSC345-02
    Lab 6 Ex 2
    3/18/2021  */

/* starvation free version */

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <strings.h>
#include <sys/types.h>


#define MAX_WAIT 3

/* farmer struct */
typedef struct _FARMER 
{
    pthread_t t;
    int isNorth;
    int idx;
    int waitfor;
} FARMER;

/* bridge */
pthread_mutex_t c_mutex;
pthread_cond_t c_condv;

int num_waiting_north = 0;
int num_waiting_south = 0;
int prev = 0;
int on_bridge = 0;

//const char* bound[2] = {"South", "North"};

void enter_bridge_n(char* bound, int idx) // some params needed here and for the exits
{
    printf("M1: %s farmer #%d arrived at the bridge\n", bound, idx);

    ++num_waiting_north;
    pthread_mutex_lock(&c_mutex);

    printf("M2: %s farmer #%d has entered the bridge\n", bound, idx);

    // some synch code with peterson solution idea
    while (on_bridge || (prev == 0 && num_waiting_south > 0))
    {
        pthread_cond_wait(&c_condv, &c_mutex);
    }

    --num_waiting_north;
    prev = 0;       // previous is south
    on_bridge = 1;  // bridge is currently occupied
}

void enter_bridge_s(char* bound, int idx) 
{
    printf("M1: %s farmer #%d arrived at the bridge\n", bound, idx);

    ++num_waiting_south;
    pthread_mutex_lock(&c_mutex);

    printf("M2: %s farmer #%d has entered the bridge\n", bound, idx);

    /* Synchronization using Peterson's Solution */
    while (on_bridge || (prev == 1 && num_waiting_north > 0))
    {
        pthread_cond_wait(&c_condv, &c_mutex);
    }

    --num_waiting_south;
    prev = 1;       // previous is north
    on_bridge = 1;  // bridge is currently occupied
}

void exit_bridge(char* bound, int idx)
{
    printf("M5: %s farmer %d has left the bridge\n", bound, idx);

    on_bridge = 0;

    /* Notify everyone waiting to pass the bridge regardless of direction */
    pthread_cond_broadcast(&c_condv);
    pthread_mutex_unlock(&c_mutex);
}

void* pass_bridge(void* param)
{
    /* 
    Each farmer thread will check if bridge is available or not. 
    If available, enter bridge. wait for some time, then exit bridge. 
    */

    FARMER* f = (FARMER*)param;

    /* Attempt to enter bridge */

    char* bound = f->isNorth ? "North" : "South";
    
    if (f->isNorth) 
        enter_bridge_n(bound, f->idx);
    else            
        enter_bridge_s(bound, f->idx);
    
    /* Wait some time */
    printf("    M3: %s farmer #%d will pass the bridge in %d seconds\n", bound, f->idx, f->waitfor);
    sleep(f->waitfor);

    /* Exit bridge */
    printf("    M4: %s farmer #%d has finished passing the bridge\n", bound, f->idx);

    exit_bridge(bound, f->idx);

    /* Exit thread */
    pthread_exit(0);
}

int main(int argc, char** argv)
{
    /* Hold number of farmers from each direction */
	int i, nNorthFarmers, nSouthFarmers;
	FARMER* f_north;
	FARMER* f_south;

	if(argc != 3) 
    {
		printf("Usage: lab06_ex1 #NorthBoundFarmers #SouthBoundFarmers\n");
	}

	nNorthFarmers = atoi(argv[1]);
	nSouthFarmers = atoi(argv[2]);
	
	if(nNorthFarmers <= 0 || nSouthFarmers <= 0)
	{
        printf("Error: Number of farmers given is not a valid number\n");
        return 1;
	}

	f_north = (FARMER*)malloc(sizeof(FARMER) * nNorthFarmers);
	f_south = (FARMER*)malloc(sizeof(FARMER) * nSouthFarmers);

    printf("* We have %d farmers from north and %d farmers from south *\n", nNorthFarmers, nSouthFarmers);


    /* Initialize mutex lock and conditional */
    pthread_cond_init(&c_condv, NULL);
    pthread_mutex_init(&c_mutex, NULL);

    for (int i=0;i<nNorthFarmers;++i)
    {
        f_north[i].isNorth = 1;
        f_north[i].idx     = i;
        f_north[i].waitfor = rand() % MAX_WAIT;
        pthread_create(&(f_north[i].t), 0, pass_bridge, &(f_north[i]));
    }
    for (int i=0;i<nSouthFarmers;++i)
    {
        f_south[i].isNorth = 0;
        f_south[i].idx     = i;
        f_south[i].waitfor = rand() % MAX_WAIT;
        pthread_create(&(f_south[i].t), 0, pass_bridge, &(f_south[i]));
    }

    for (int i=0;i<nNorthFarmers;++i) 
    {
        pthread_join(f_north[i].t, NULL);
    }
    for (int i=0;i<nSouthFarmers;++i) 
    {
        pthread_join(f_south[i].t, NULL);
    }

    /* Finishing the mutex lock and conditional */
    pthread_mutex_destroy(&c_mutex);
    pthread_cond_destroy(&c_condv);

    printf("* All farmers passed *\n");

    /* Free up allocated memory */
    free(f_north);
    free(f_south);

    return 0;
}