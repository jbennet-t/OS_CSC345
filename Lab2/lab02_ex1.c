// Jordan Sinoway
// CSC345-02
// Lab 2 Ex 1
// 2/11/2021

#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <wait.h>

int value = 5;

int main()
{
    pid_t pid;
    pid = fork();
    if (pid == 0){
        value += 15;
        return 0;
    }
    else if(pid > 0){
        wait(NULL);
        printf("PARENT val=%d\n", value);
        return 0;
    }
}