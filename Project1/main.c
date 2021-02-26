// Jordan Sinoway
// CSC345-02
// Project 1
// 2/26/2021

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <wait.h>

#define MAX_LINE 80 /*maximum length command*/

int main(void)
{
    char *args[MAX_LINE/2+1]; /* command line arguments */
    char str[MAX_LINE/2+1]; /* temp for cmd line input, not a pointer so it can be stepped through */
    char *cmd; /* holds arguements from cmd line*/
    char *token; /* holds cmd after being tokenized */
    char *history = "0"; /* holds prev command */

    int should_run = 1; /* flag for exiting program */
    int len; /* input command length */
    int argc; /* argument counter */
    int background_process; /* flag for background process "&" */
    int redirect_out; /* lag for file redirection out ">"" */
    int redirect_in; /*flag for file redirection in "<" */
    int pipe_process; /*flag for pipe operator "|" */ 
    int pipefd[2]; /*pipe file descriptor*/


    pid_t pid; /*process id*/
    pid_t pid_pipe; /*pipe process id*/


    while(should_run)
    {
        background_process = 0;
        redirect_out = 0;
        redirect_in = 0;
        pipe_process = 0;
        argc = 0;

        printf("osh>");
        fflush(stdout);

        /*getting user input */
        fgets(str,MAX_LINE,stdin); /*reading # of MAX_LINE charcters from stdin, and storing them into str */
        len = strlen(str);


        /* exit case */
        if(strcmp(str, "exit\n") == 0)
        {
            printf("Exited Shell\n");
            exit(0);
        }

        /* set newline charcter from \n to NULL */
        if(str[len-1] == '\n')
        {
            str[len-1] = '\0';
        }

        /* preallocate mem for command string */
        cmd = malloc(sizeof(str) * sizeof(char)); // 41 * 1 byte

        /* check for history command,"!!" */
        if(strcmp(str, "!!") == 0)
        {
            if(strcmp(history, "0") == 0) /* if nothing in history */
            {
                printf("No commands in history.\n");
            }

            cmd = history; /* last thing stored in history variable is now current cmd */
            history = malloc((MAX_LINE/2+1) * sizeof(char)); /* sets history to pointer */
            memcpy(history, cmd, strlen(cmd)); /*stores new cmd in history location */
        }
        else if(str[0] != '\n' && str[0] != '\0') /*if not history cmd, proceed normally */
        {
            cmd = str;
            history = malloc((MAX_LINE/2+1) * sizeof(char));
            memcpy(history, str, len);
        }
       

        /* tokenize command, breaking up cmd into chunks, delineated by " " */
        token = strtok(cmd," ");

        while(token != NULL)
        {
            /* set each token to a different element of args */
            args[argc] = token;

            if(strcmp(token,"|") == 0)
            {
                pipe_process = argc;
            }

            token = strtok(NULL, " "); /* clearing token*/
            argc++;
        }

        args[argc] = '\0'; /* set last arguement to NULL so execvp() terminates*/

        /* directory change */
        if(strcmp(args[0], "cd") == 0) /*if first token == cd */
        {
            if(chdir(args[1]) == 0) /* if valid cd */
            {
                chdir(args[1]); /* cd to 2nd token */
            }
            else
            {
                perror("Invalid cd");
            }
        }
        
        /* background process - if last argument is "&"" */
        if (strcmp(args[argc-1],"&") == 0) 
        {
            background_process = 1;
            args[argc-1] = '\0'; /* clearing last argument */
        }

        /* redirection handling - if 2nd to last argument is outputting to .txt "> out.txt" */
        if (argc > 2)
        {
            if (strcmp(args[argc-2],">") == 0)
            {
                args[argc-2] = '\0';
                redirect_out = 1;
            } 
            else if (strcmp(args[argc-2],"<") == 0) 
            {
                args[argc-2] = '\0';
                redirect_in = 1; 
            }
        }
        
        /* parent & child forking */
        pid = fork();
        
        if(pid == 0)/* Child process 1 */
        {
            if(argc > 0) /* if theres arguments */
            {
                if(redirect_out) /* redirecting data out */
                {
                    int fdOut;
                    if((fdOut = creat(args[argc-1], 0644)) < 0)
                    {
                        printf("File error - could not create file.\n");
                        exit(0);
                    }
                    dup2(fdOut, STDOUT_FILENO);
                    close(fdOut);
                }
                else if(redirect_in) /* redirecting data in */
                {
                    int fdIn;
                    if((fdIn = open(args[argc-1], O_RDONLY)) < 0 )
                    {
                        printf("File error - could not open file.\n");
                        exit(0);
                    }
                    dup2(fdIn, STDIN_FILENO);
                    close(fdIn);
                }

                /* "|" pipe allows output from 1st command to serve as input to another */
                /* assuming no redirect op at same time */
                if(pipe_process)
                {
                    /* create pipe and handle errors*/
                    if(pipe(pipefd) < 0)
                    {
                        printf("Failed to create the pipe.\n");
                        exit(1);
                    }

                    /* fork new process for the child to execute */
                    pid_pipe = fork();

                    if(pid_pipe == 0) /* Child process #2 */
                    {
                        /* redirection for STDOUT to pipe */
                        dup2(pipefd[1], STDOUT_FILENO);
                        args[pipe_process] = '\0';
                        close(pipefd[0]);

                        /* execute first argument */
                        execvp(args[0],args);
                    }
                    else if (pid_pipe > 0) /* parent process #2 */
                    {
                        wait(NULL);

                        /* redirection for STDIN to pipe */
                        dup2(pipefd[0],STDIN_FILENO);
                        close(pipefd[1]);

                        /* execute args after the pipe "|" */
                        execvp(args[pipe_process + 1], &args[pipe_process + 1]);
                    }
                }
                else /* invoke execvp() */
                    execvp(args[0],args); /* executing all other commands, i.e ls,date,...etc. */
            }
            else
            {
                fprintf(stderr,"You have zero arguments.\n");
            }
        }
        else if(pid > 0) /* Parent process #1 */
        {
            /* invoke wait unless command included & */
            if(!background_process)
            {
             wait(NULL);
            }
        }
        
    }
    return 0;
}
