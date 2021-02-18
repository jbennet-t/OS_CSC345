#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int len;
int* nums;

int r_min;
int r_max;

void* myMin(void* param)
{
    int i;
    r_min = nums[0];
    for (i = 0; i < len; ++i)
    {
        if(r_min > nums[i])
            r_min = nums[i];
    }
    pthread_exit(0);
}

void* myMax(void* param)
{
    int i;
    r_max = nums[0];
    for (i = 0; i < len; ++i)
    {
        if(r_max < nums[i])
            r_max = nums[i];
    }
    pthread_exit(0);
}

float r_avg;
void* myAvg(void* param)
{
    int i;
    r_avg = 0.0f;
    for(i = 0; i < len; ++i)
    {
        r_avg += nums[i];
    }
    r_avg /= len;
    pthread_exit(0);
}

int main(int argc, char** argv)
{
    int i;
    len = argc - 1;
    nums = (int*)malloc(sizeof(int)*len); //preparing memory space
    for(i = 0; i<len; ++i)
    {
        nums[i] = atoi(argv[i+1]);
    }

    //add median and standard deviation
    pthread_t tid1;
    pthread_t tid2;
    pthread_t tid3;
    pthread_create(&tid1, 0, myMin, NULL);
    pthread_create(&tid2, 0, myMax, NULL);
    pthread_create(&tid3, 0, myAvg, NULL);
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    pthread_join(tid3, NULL);
    printf("Min = %d\n", r_min);
    printf("Max = %d\n", r_max);
    printf("Avg = %f\n", r_avg);

    free(nums); //freeing memory after use
    return 0;
}