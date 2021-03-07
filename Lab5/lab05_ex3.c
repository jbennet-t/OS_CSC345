// Jordan Sinoway
// CSC345-02
// Lab 5 Ex 3
// 3/7/2021

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

#define NUM_THREADS 4

long points_inCircle = 0;
long points_total;
int points_per_thread;
int thread_count;
double pi;


pthread_mutex_t lock;

void* run(void* pointer_val)
{
    double x,y;
    int cnt = (*(int*)pointer_val);

    for (int i=0;i < cnt;++i)
    {
        // generate points_total points in square - numbers between -1 and +1
        x = 2.0 * ((double)rand() / (double)RAND_MAX) - 1;
        y = 2.0 * ((double)rand() / (double)RAND_MAX) - 1;

        // count how many of them fall within circle of r = 1
        if (sqrt(x*x + y*y) < 1.0)
        {
            pthread_mutex_lock(&lock);
            points_inCircle++;
            pthread_mutex_unlock(&lock);
        }
    }
    thread_count++;
    printf("# of points calculated in thread %d = %d\n", thread_count, cnt);
    pthread_exit(0);
}

int main(int argc, char** argv)
{
    // get total number of random points to be generated (from command line)
    points_total = atol(argv[1]);
    int i;

    // create thread to do the counting job
    pthread_t tid[NUM_THREADS];

    pthread_mutex_init(&lock, NULL);

    points_per_thread = (points_total/NUM_THREADS);
    int last_thread_pts = (points_total/NUM_THREADS) + (points_total%NUM_THREADS);
    

    for (i=0;i<NUM_THREADS-1;++i) 
    {
        pthread_create(&tid[i], 0, run, &points_per_thread); 
    }
    pthread_create(&tid[i], 0, run, &last_thread_pts); 


    for (i=0;i<NUM_THREADS;++i)
    {
        pthread_join(tid[i], NULL);
    }

    // estimate pi based on the counting result

    pi = 4.0 * (double)points_inCircle / (double)points_total;

    printf("Estimated pi = %f\n", pi);

    pthread_mutex_destroy(&lock);

    return 0;
}