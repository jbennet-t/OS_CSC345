#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define MAX_LINE 80 /*maximum length command*/

int main(void)
{
    char *args[MAX_LINE/2+1]; /* command line arguements */
    int should_run = 1; /* flag to determine when to exit programs */


    while(should_run)
    {
        printf("osh>");
        fflush(stdout);

        
    }

    return 0;
}