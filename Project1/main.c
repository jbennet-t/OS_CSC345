// Jordan Sinoway
// CSC345-02
// Project 1
// 2/26/2021

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define MAX_LINE 80 /*maximum length command*/

int main(void)
{
    char *args[MAX_LINE/2+1]; /* command line arguments */
    char str[MAX_LINE/2+1];
    char *cmd;

    int should_run = 1; /* flag to determine when to exit programs */
    int argc;
    int len;


    
    pid_t pid; /*process id*/
    pid_t pid_pipe; /*pipe process id for pipe*/

    while(should_run)
    {
        // background_process = 0;
        // redirect_out = 0;
        // redirect_in = 0;
        // pipe_process = 0;
        argc = 0;

        printf("osh>");
        fflush(stdout);

        /*getting user input */
        fgets(str,MAX_LINE,stdin); /*reading # of MAX_LINE charcters from stdin, and storing them into str */
        len = strlen(str);

        /* set newline charcter from \n to NULL */
        if(str[len-1] == '\n')
        {
            str[len-1] = '\0';
        }

        /* allocate mem for command string */
        cmd = malloc(MAX_LINE/2+1 * sizeof(char));








        
    }

    return 0;
}