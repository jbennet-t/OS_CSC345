/*
* Jordan Sinoway and Michael Ralea
* CSC345-02
* 4/9/2021
* Project 3 - Virtual Memory Manager - main_pr.c
* with page replacement
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <time.h>

#define MAX_PT_ENTRIES 256      /* max page table entries */
#define PAGE_SIZE 256           /* page size in bytes*/
#define MAX_TLB_ENTRIES 16      /* max TLB entries */
#define FRAME_SIZE 256          /* frame size in bytes*/
#define MAX_FRAMES 256          /* maxi frames in physical memory */

#define BYTES_PER_INPUT 256     /* # of bytes to read from BACKING_STORE.bin */

typedef struct
{
    int page_num;
    int frame_num;
} page_frame;

int physical_memory[MAX_FRAMES][FRAME_SIZE]; /* Physical memory holds 256 bytes per frame */
page_frame TLB[MAX_TLB_ENTRIES];             /* TLB holds a page number and a cooresponding frame number */
page_frame page_table[MAX_PT_ENTRIES];       /* Page table holds a page number and a cooresponding frame number */

int address_cnt = 0;   /* translated address counter */
int page_faults_cnt = 0;/* page fault counter */
int tlb_hits = 0;       /* TLB hit counter */
int framePos = 0;       /* next available frame position in physical memory */
int PTPos = 0;          /* next available position in page table */
int TLBPos = 0;         /* next available position in TLB */

/* Load BACKING_STORE.bin - mirror of everything in logical address space */
FILE *backing_store;
/* Create output files for virtual addresses, physical addresses, and values */
FILE *virtual, *physical, *val;

void getPage(int address); /*instantiate functions */
void readStore(int page_num);
void TLBInsert(int page_num, int frame_num);

int main(int argc, char** argv)
{
    /* Load addresses from input file, as directed from command line*/
    FILE *input;
    if(argc == 2) 
    {
        char const* const fileName = argv[1]; /* correct usage: ./main addresses.txt */
        input = fopen(fileName, "rt");

        /* open up output files */
        virtual = fopen("out1.txt","w");
        physical = fopen("out2.txt","w");
        val = fopen("out3.txt","w");
    }
    else
    {
        fprintf(stderr,"Input address file not specified.\n"); /* error checking input */
        return -1;
    }

    int32_t logical_address;
    fscanf(input, "%d", &logical_address); /*logical address */

    while(!feof(input)) /*pull logical addresses from the address file*/
    {   
        address_cnt++;
        /* get physical address and value stored at that address */
        getPage(logical_address);

        /* Scan for any new integers on proceeding lines */
        fscanf(input, "%d", &logical_address);
    }
    printf("%d addresses.\n", address_cnt);

    fclose(input);
    fclose(virtual);
    fclose(physical);
    fclose(val);

    double pf_rate = page_faults_cnt / (double)address_cnt;
    double tlb_rate = tlb_hits / (double)address_cnt;

    printf("Page Faults: %d\tPage Fault Rate: %.2f\n", page_faults_cnt, pf_rate);
    printf("TLB Hits: %d\t TLB Rate: %.3f\n", tlb_hits, tlb_rate);

    return 0;
}

void getPage(int logical_address)
{
    /* Declare page (0x0000FF00) and offset (0x000000FF) */
    uint8_t page = logical_address >> 8 & 0xFF; 
    uint8_t offset = logical_address & 0xFF;
    int frame = -1;
    int i;

    /* Search TLB for page and set frame number if found */
    for(i = 0; i < TLBPos; i++)
    {
        if(TLB[i].page_num == page)
        {
            frame = TLB[i].frame_num;
            tlb_hits++;
            break;
        }
    }

    /* If not found, search page table and set frame number if found */
    if(frame == -1)
    {
        for(i = 0;i < PTPos; i++)
        {
            if(page_table[i].page_num == page)
            {
                /* Page was found - get frame */
                frame = page_table[i].frame_num;
                break;
            }
        }
    }

    /* If not found, declare page fault and read in frame number from backing_store (readFromStore) */
    /* Update page table entry, restart address conversion and access procedure */
    if(frame == -1)
    {
        readStore(page);
        page_faults_cnt++;
        /* Decrement updated framePos to get frame of current page */
        frame = framePos - 1;
    }

    /* Insert page num and frame into TLB if not there already */
    TLBInsert(page, frame);

    fprintf(virtual,"%d\n",logical_address); /* logical_address -> out1.txt */
    int physical_address = frame * PAGE_SIZE + offset;
    fprintf(physical,"%d\n",physical_address); /* frame_num * 256 -> out2.txt */
    int8_t value = physical_memory[frame][offset];
    fprintf(val,"%d\n",value); /* Get signed byte value stored in physical_memory using frame_num and offset -> out3.txt */
}

void readStore(int page_num)
{
    /* Seek for page_num in backing_store */
    /* Bring cooresponding frame num to physical_memory and page_table */

    /* Declare buffer to hold 256 bytes */
    int8_t buf[256];           
    int i;

    /* open BACKING_STORE.bin */
    backing_store = fopen("BACKING_STORE.bin","rb");

    /* Set file pointer at page number position */
    if(fseek(backing_store, page_num*BYTES_PER_INPUT, SEEK_SET) != 0)
    {
        fprintf(stderr,"Error seeking through backing_store\n");
    }

    /* Read 256 bytes to buffer array */
    if(fread(buf, sizeof(int8_t), BYTES_PER_INPUT, backing_store) == 0)
    {
        fprintf(stderr, "Error reading through file\n");
    }

    fclose(backing_store); /* close BACKING_STORE.bin */

    /* if physical memory has not filled */
    if(framePos < MAX_FRAMES)
    {
        /* Update frame in physical memory with 256 bytes */
        for(i = 0;i < BYTES_PER_INPUT; ++i)
        {
            physical_memory[framePos][i] = buf[i];
        }

        /* Update page table with page and frame */
        page_table[PTPos].page_num = page_num;
        page_table[PTPos].frame_num = framePos;

        /* Increment frame and page table's next available position */
        framePos++;
        PTPos++;
    }
    else /* Physical memory/PT is filled - use FIFO for replacement */
    {
        int j;
        for(j = 0;j < MAX_FRAMES-1; j++) 
        {
            /* Push top mem out of queue */
            for(i = 0; i < BYTES_PER_INPUT; ++i) 
            physical_memory[j][i] = physical_memory[j+1][i];  
            /* Push top page out of queue */
            page_table[j] = page_table[j+1];            
        }

        /* Add new memory to back of physical memory */
        for(i = 0;i < BYTES_PER_INPUT; ++i) 
        physical_memory[j][i] = buf[i];

        /* Add new page and frame to back of page table */
        page_table[j].page_num = page_num;             
        page_table[j].frame_num = framePos-1;
    }
}

void TLBInsert(int page_num, int frame_num) /* Insert page and frame into TLB */
{
    int i, j;

    /* Break if already on the TLB and save index */
    for(i = 0; i < TLBPos; ++i)
    {
        if(TLB[i].page_num == page_num) 
        break;
    }

    /* If page number was not found in TLB */
    if(i == TLBPos)
    {
        /* Insert to TLB if there is still room */
        if(TLBPos < MAX_TLB_ENTRIES)
        {
            TLB[TLBPos].page_num = page_num;
            TLB[TLBPos].frame_num = frame_num;
        }
        else /* Otherwise move everything over and insert page frame on end */
        {
            for(i = 0; i < MAX_TLB_ENTRIES-1; i++)
            {
                TLB[i] = TLB[i+1];
            }
            /* Insert new page frame */
            TLB[MAX_TLB_ENTRIES-1].page_num = page_num;
            TLB[MAX_TLB_ENTRIES-1].frame_num = frame_num;
        }
    }
    else /* If page num was found in TLB */
    {
        /* Increment position of everything starting from page index */
        for(i = i; i < MAX_TLB_ENTRIES-1; i++)
        {
            TLB[i] = TLB[i+1];
        }
        /* If there is still room, put page and frame on the end */
        if(TLBPos < MAX_TLB_ENTRIES)
        {
            TLB[TLBPos].page_num = page_num;
            TLB[TLBPos].frame_num = frame_num;
        }
        else /* Otherwise, place page and frame on num entries - 1 */
        {
            TLB[TLBPos-1].page_num = page_num;
            TLB[TLBPos-1].frame_num = frame_num;
        }
    }
    if(TLBPos < MAX_TLB_ENTRIES)
    {
        TLBPos++;
    }
}

