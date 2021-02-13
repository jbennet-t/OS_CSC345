// Jordan Sinoway
// CSC345-02
// Lab 2 Ex 2
// 2/11/2021

#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <wait.h>

#define SIZE 5
int nums[SIZE] = {0,1,2,3,4};

int main()
{
    int i;
    pid_t pid;
    pid = fork();

    if(pid == 0){
        for(i = 0; i < SIZE; i++){
            nums[i] *= -i;
            printf("CHILD: %d\n", nums[i]);
        }
    }
    else if(pid < 0) {
        wait(NULL);
        for(i = 0; i < SIZE; i++){
            printf("PARENT: %d\n", nums[i]);

        }
    }
    return 0;
}