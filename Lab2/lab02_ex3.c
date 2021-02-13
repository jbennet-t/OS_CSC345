// Jordan Sinoway
// CSC345-02
// Lab 2 Ex 3
// 2/11/2021

#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <wait.h>

int main()
{
    pid_t pid = fork();

    if(pid == 0){
        //child
        //do nothing

    }
    else if (pid > 0){
        //parent
        //wait(NULL)
        sleep(20);

    }
    return 0;
}