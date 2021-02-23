// Jordan Sinoway
// CSC345-02
// Lab 3 Ex 2
// 2/23/2021

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

int total_pts;
int pts_in_circle;


void* run(void* param)
{
    int i;
    for(int i = 0; i < total_pts; ++i)
    {

        double x = (double)rand() / (double)RAND_MAX *2.0 - 1.0;
        double y = (double)rand() / (double)RAND_MAX *2.0 - 1.0;


        if (sqrt(x * x + y * y) < 1.0)
        {
            ++pts_in_circle;
        }
    }
     pthread_exit(0);
}


int main(int argc, char** argv)
{
    total_pts = atoi(argv[1]);

    pthread_t tid;
    pthread_create(&tid, 0, run, NULL);
    pthread_join(tid, NULL);

    double pi_est = 4.0 * (double)pts_in_circle / (double)total_pts;
    printf("PI = %f\n", pi_est);

    return 0;
}