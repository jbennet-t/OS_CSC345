/*  Jordan Sinoway
    CSC345-02
    Lab 6 Ex 1
    3/11/2021  */

    /* this example includes starvation: i.e. if there are 4 farmers from North and 1 from south,
    all the farmers from the North may pass first without allowing the south bound farmer through*/

#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <strings.h>
#include <semaphore.h>

#define MAX_WAIT 3 

/* farmer struct to hold individual farmer params */
typedef struct _FARMER {
	pthread_t t;
	int isNorth;
	int idx;
	int waitfor;
}
FARMER;

sem_t sem; //bridge

/* enters bridge and waits random amount of time */
void enter_bridge(char* bound, int idx)
{
    printf("M1: %s farmer #%d is about to enter the bridge\n", bound, idx);
    sem_wait(&sem);
    printf("M2: %s farmer #%d has entered the bridge\n", bound, idx);
}

/* exiting bridge */
void exit_bridge(char* bound, int idx)
{
	printf("M5: %s farmer #%d has left the bridge\n", bound, idx);
	sem_post(&sem);
}

/*checks params to see farmer's direction */
void* pass_bridge(void* param)
{
	FARMER* f = (FARMER*) param;
	char* bound = f->isNorth ? "North" : "South";
	
	enter_bridge(bound, f->idx);

	printf(" M3: %s farmer #%d will pass the bridge in %d seconds\n", bound, f->idx, f->waitfor);
	sleep(f->waitfor);
	printf(" M4: %s farmer #%d has finished passing the bridge\n", bound, f->idx);

	exit_bridge(bound, f->idx);
}

int main(int argc, char** argv)
{
	int i, nNorthFarmers, nSouthFarmers;
	FARMER* f_north;
	FARMER* f_south;

	if(argc != 3) /*if incorrect input */
    {
		printf("Usage: lab06_ex1 #NorthBoundFarmers #SouthBoundFarmers\n");
	}

	nNorthFarmers = atoi(argv[1]); /* taking in # of N/S farmers as arguements */
	nSouthFarmers = atoi(argv[2]);
	
	if(nNorthFarmers <= 0 || nSouthFarmers <= 0) /* if invalid # of farmers */
	{
        printf("Error: Number of farmers given is not a valid number\n");
        return 1;
	}

	f_north = (FARMER*)malloc(sizeof(FARMER) * nNorthFarmers);
	f_south = (FARMER*)malloc(sizeof(FARMER) * nSouthFarmers);

	printf("* We have %d farmers from north and %d farmers from south *\n", nNorthFarmers, nSouthFarmers);

	sem_init(&sem, 0, 1);

	for(i = 0; i < nNorthFarmers; ++i)
	{
        f_north[i].isNorth = 1;
        f_north[i].idx = i;
        f_north[i].waitfor = rand() % MAX_WAIT; /* assign random wait and other params */
        pthread_create(&(f_north[i].t), 0, pass_bridge, &(f_north[i])); /* creating thread for each farmer */
	}

	for(i = 0; i < nSouthFarmers; ++i)
	{
        f_south[i].isNorth = 0;
        f_south[i].idx = i;
        f_south[i].waitfor = rand() % MAX_WAIT; /* assign random wait and other params */
        pthread_create(&(f_south[i].t), 0, pass_bridge, &(f_south[i])); /* creating thread for each farmer */
	}

	for(i = 0; i < nNorthFarmers; ++i)
	{
	    pthread_join(f_north[i].t, NULL); /* joining threads after completion */
	}

	for(i = 0; i < nSouthFarmers; ++i)
	{
	    pthread_join(f_south[i].t, NULL);
	}

    /* Finishing the semaphore */
	sem_destroy(&sem);

	printf("* All farmers passed *\n");
	
    /* Free up allocated memory */
	free(f_north);
	free(f_south);

return 0;
}
