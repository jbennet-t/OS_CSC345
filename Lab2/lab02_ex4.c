// Jordan Sinoway
// CSC345-02
// Lab 2 Ex 4
// 2/13/2021

#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <wait.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <fcntl.h>


int main(int argc, char** argv)
{
    const char* name = "COLLATZ"; //name of shared mem object
    const int SIZE = 4096; //size of shared mem object
    int n = atoi(argv[1]); //taking in # for Collatz algorithm

    void *ptr;
    int shm_fd;

    shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666); //create shared mem obj
    ftruncate(shm_fd, SIZE); //configure shared mem obj size
    ptr = mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0); //memory map shared mem obj

    pid_t id = fork();

    if(id == 0)
    {
        //PRODUCER - child
        while(n > 1) 
        {
            sprintf(ptr, "%d", n);
            ptr++;
            if(n % 2 == 0)
            {
                n = n/2;
            }
            else
            {
                n = 3 * n + 1;
            }
        }
        sprintf(ptr, "%d", n);
        ptr++;

    }
    else
    {
        //CONSUMER - parent
        wait(NULL); //waiting for child process to finish

        printf("%s\n",(char *)ptr); //read and print from shared mem object

        shm_unlink(name); //remove the shared mem obj


    }
    return 0;
}

/* 
int main(int argc, char** argv)
{
    int n = atoi(argv[1]); //must provide an input for this program, i.e. ./lab02_ex4 8
    pid_t id = fork();
    if(id == 0)
    {
        while(n > 1) 
        {
            printf("%d ", n);
            if(n % 2 == 0)
            {
                n = n/2;
            }
            else
            {
                n = 3 * n + 1;
            }
        }
        printf("%d\n", n);
    }
    else
    {
        wait(NULL);
    }
    return 0;
} */
