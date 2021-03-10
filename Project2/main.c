#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

/* Create struct to pass into each thread as parameters */
typedef struct
{
    int row; /* row index*/
    int col; /* columm index*/

} parameters;

/*
void *check_grid(void *params)
{

}
*/

/*
void *check_row(void *params)
{

}
*/

/*
void *check_all_row(void *params)
{

}

/*
void *check_col(void *params)
{

}
*/

/*
void *check_all_col(void *params)
{

}
*/


int main(int argc, char** argv)
{
    int option = atoi(argv[1]);
    int num_threads; /*selecting number of threads via user input for option*/
    
    printf("BOARD STATE IN input.txt\n");

    FILE *input;
    input = fopen("input.txt","r");

    /* Initialize board */
    int sudoku_table[9][9], i, j;


    /* Read and assign sudoku board from input.txt */
    for (i=0;i<9;i++)
    {
        for (j=0;j<9;j++)
        {
            fscanf(input, "%1d", &sudoku_table[i][j]);
            printf("%d ", sudoku_table[i][j]);
        }
        printf("\n");
    }
    

    if(option == 1)
        num_threads == 11; 
    else if(option == 2) /* 9 threads check grids, 1 thread checks rows, 9 threads check cols*/
        num_threads == 19;
    else if(option == 3) /* 9 threads check grids, 9 threads check rows, 9 threads check cols*/
        num_threads == 27;

    


    return 0;
}