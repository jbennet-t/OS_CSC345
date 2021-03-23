/*  Jordan Sinoway
    CSC345-02
    Lab 6 Ex 1
    3/11/2021  */

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <strings.h>
#include <sys/types.h>

/* farmer */
#define MAX_WAIT 3

typedef struct _FARMER 
{
    pthread_t t;
    int isNorth;
    int idx;
    int waitfor;
} FARMER;

/* bridge */
sem_t sem;

void enter_bridge(char* bound, int idx)
{
    printf("M1: %s farmer #%d is about to enter the bridge\n", bound, idx);
    sem_wait(&sem);
    printf("M2: %s farmer #%d has entered the bridge\n", bound, idx);
}

void exit_bridge(char* bound, int idx)
{
    printf("M5: %s farmer #%d has left the bridge\n", bound, idx);
    sem_post(&sem);
}

void* pass_bridge(void* param)
{
    /* 
    each farmer aka thread will check if bridge is available or not. 
    If available, enter bridge. 
    wait for some time, 
    then exit bridge,
    done 
    */

    FARMER* f = (FARMER*)param;

    char* bound = f->isNorth ? "North" : "South";

    enter_bridge(bound, f->idx);

    printf("    M3: %s farmer #%d will pass the bridge in %d seconds\n", bound, f->idx, f->waitfor);
    sleep(f->waitfor);
    printf("    M4: %s farmer #%d has finshed passing the bridge\n", bound, f->idx);

    exit_bridge(bound, f->idx);
}

int main(int argc, char** argv)
{
    /* Hold number of farmers from each direction */
    int nNorthFarmers = atoi(argv[1]);
    int nSouthFarmers = atoi(argv[2]);
    int i;

    //sample input: ./lab06_ex1 1 5

    /* list of farmers */
    FARMER* f_north = (FARMER*)malloc(sizeof(FARMER) * nNorthFarmers);
    FARMER* f_south = (FARMER*)malloc(sizeof(FARMER) * nSouthFarmers);

    if(argc != 3)
    {
        printf("Usage: lab06_ex1 #NorthBoundFarms #SouthBoundFarmers\n");
        return 1;
    }

    if(nNorthFarmers <= 0 || nSouthFarmers <= 0)
    {
        printf("Error: Number of farmers is not a valid number \n");
        return 1;
    }

    printf("* We have %d farmers from north and %d farmers from south *\n", nNorthFarmers, nSouthFarmers);

    sem_init(&sem, 0, 1);

     for (int i = 0;i < nNorthFarmers; ++i)
    {
        f_north[i].isNorth = 1;
        f_north[i].idx     = i;
        f_north[i].waitfor = rand() % MAX_WAIT;
        pthread_create(&(f_north[i].t), 0, pass_bridge, &(f_north[i]));
    }
    for (int i = 0;i < nSouthFarmers; ++i)
    {
        f_south[i].isNorth = 0;
        f_south[i].idx     = i;
        f_south[i].waitfor = rand() % MAX_WAIT;
        pthread_create(&(f_south[i].t), 0, pass_bridge, &(f_south[i]));
    }

    for (int i = 0; i < nNorthFarmers; ++i)
    {
        pthread_join(f_north[i].t, NULL);
    }

    for (int i = 0; i < nSouthFarmers; ++i)
    {
        pthread_join(f_south[i].t, NULL);
    }

    sem_destroy(&sem);

    printf("* All farmers passed *\n");

    free(f_north);
    free(f_south);

    return 0;
}