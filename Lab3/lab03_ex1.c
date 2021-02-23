#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

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

float r_med;
void *myMedian(void *param)
{
    int i, j, temp;

    for (i=0;i<len;++i) //reordering nums
    {
        for (j=i+1;j<len;++j)
        {
            if (nums[i] > nums[j])
            {
                temp = nums[i];//swap numbers
                nums[i] = nums[j];
                nums[j] = temp;
            }
        }
    }               // [1,2,3,4,5,6,7] = 7

    if (len % 2 == 1)//if odd
    {
        r_med = nums[(len-1)/2];//median = middle num
    }
    else //if even
    {
        float med1 = nums[len/2 - 1];
        float med2 = nums[len/2];

        r_med = (med1 + med2)/2; //take avg of middle vals
    }

    pthread_exit(0);
}

float r_stndev;
void* myStndev(void* param)
{
    int i = 0;
    int sum = 0;
    float mean = 0;

    for (i=0;i<len;++i)
    {
        mean += nums[i];
    }
    mean /= len;

    for (i=0;i<len;++i)
    {
        sum += pow(nums[i] - mean, 2); //sum of differences^2
    }
    r_stndev = sqrt(sum / len);//sqrt(variance) = stddev

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
    pthread_t tid4;
    pthread_t tid5;
    pthread_create(&tid1, 0, myMin, NULL);
    pthread_create(&tid2, 0, myMax, NULL);
    pthread_create(&tid3, 0, myAvg, NULL);
    pthread_create(&tid4, 0, myMedian, NULL);
    pthread_create(&tid5, 0, myStndev, NULL);
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    pthread_join(tid3, NULL);
    pthread_join(tid4, NULL);
    pthread_join(tid5, NULL);
    printf("Min = %d\n", r_min);
    printf("Max = %d\n", r_max);
    printf("Avg = %f\n", r_avg);
    printf("Med = %f\n", r_med);
    printf("Stndev = %f\n", r_stndev);

    free(nums); //freeing memory after use
    return 0;
}