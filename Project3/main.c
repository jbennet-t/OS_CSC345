/*
* Jordan Sinoway and Michael Ralea
* CSC345-02
* November 8 2019
* Project 3 - Virtual Memory Manager
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <time.h>

#define MAX_PT_ENTRIES 256      /* Maximum page table entries */
#define PAGE_SIZE 256           /* Page size of 256 bytes */
#define MAX_TLB_ENTRIES 16      /* Maximum TLB entries */
#define FRAME_SIZE 256          /* Frame size of 256 bytes */
#define MAX_FRAMES 256          /* Maximum frames in physical memory */

#define BYTES_PER_INPUT 256     /* Chunk of bytes to read from backing_store */

typedef struct
{
    int page_num;
    int frame_num;
} page_frame;

int physical_memory[MAX_FRAMES][FRAME_SIZE]; /* Physical memory holds 256 bytes per frame */
page_frame TLB[MAX_TLB_ENTRIES];             /* TLB holds a page number and a cooresponding frame number */
page_frame page_table[MAX_PT_ENTRIES];       /* Page table holds a page number and a cooresponding frame number */

int page_faults = 0;    /* Page fault counter */
int tlb_hits = 0;       /* TLB hit counter */
int framePos = 0;       /* Next available frame position in physical memory */
int PTPos = 0;          /* Next available position in page table */
int TLBPos = 0;         /* Next available position in TLB */
int addressCount = 0;   /* Translated address counter */

/* Load BACKING_STORE.bin - mirror of everything in logical address space */
FILE *backing_store;
/* Create output files for virtual addresses, physical addresses, and values */
FILE *virtual, *physical, *val;

void getPage(int address);
void readStore(int page_num);
void TLBInsert(int page_num, int frame_num);

int main(int argc, char** argv)
{
    /* Load addresses through input file */
    FILE *input;
    if (argc == 2)
    {
        char const* const fileName = argv[1];
        input = fopen(fileName, "rt");

        /* Open output files */
        virtual = fopen("out1.txt","w");
        physical = fopen("out2.txt","w");
        val = fopen("out3.txt","w");
    }
    else
    {
        fprintf(stderr,"Input address file not specified.\n");
        return -1;
    }

    /* Declare logical address variables */
    int32_t logical_address;
    fscanf(input, "%d", &logical_address);

    fclose(input);
    fclose(virtual);
    fclose(physical);
    fclose(val);

    double pf_rate = page_faults / (double)addressCount;
    double tlb_rate = tlb_hits / (double)addressCount;

    /* Will not run out of memory with 256 frames */
    printf("Page Faults: %d\tPage Fault Rate: %.2f\n", page_faults, pf_rate);
    printf("TLB Hits: %d\t TLB Rate: %.3f\n", tlb_hits, tlb_rate);

    return 0;
}