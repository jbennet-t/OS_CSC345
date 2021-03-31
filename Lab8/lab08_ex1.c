#include <stdio.h>
#include <stdlib.h>


#define PAGE_SIZE 4096

int main(int argc, char** argv)
{
    unsigned int addr =  (unsigned int)atoi(argv[1]);
    int page = addr / PAGE_SIZE;
    int offset = addr % PAGE_SIZE;

    printf("The address %u is located at:\n", addr);
    printf("page number = %d\n", page);
    printf("offeset = %d\n", offset);

    return 0;
}