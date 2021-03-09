#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

/* Create struct to pass into each thread as parameters */
typedef struct
{
    int row;
    int col;
} parameters;

int main(int argc, char** argv)
{
    int option = atoi(argv[1]);
    int num_threads;
    /*selecting number of threads via user input for option*/


    if(option == 1)
        num_threads == 11; 
    else if(option == 2)
        num_threads == 19; /* 9 threads check cols, 9 threads, check rows, 1 master thread */
    else if(option == 3)
        num_threads == 


}