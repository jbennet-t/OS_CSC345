/*
Jordan Sinoway
CSC 345-02
lab10 - inputgen.c
*/


#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX_CYL 5000
#define NUM_REQ 1000
#define INI_POS 42

int main(int argc, char** argv)
{
    FILE* fp = fopen("input.txt", "wt");

    fprintf(fp, "%d\n", MAX_CYL);
    fprintf(fp, "%d\n", NUM_REQ);

    for(int i = 0; i < NUM_REQ; i++)
    {
        fprintf(fp, "%d\n", rand() % MAX_CYL);
    }

    fprintf(fp, "%d", INI_POS);

    fclose(fp);

    return 0;
}