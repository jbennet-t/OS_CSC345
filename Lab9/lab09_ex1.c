#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int total_frames;
//int* FRAME;

int total_refs;
int* page_refs;

int pf_fifo, pf_lru, pf_opt; /* page fault counts */


int hit(int frames[], int page)
{
    int hit = 0;
    for (int i=0; i<total_frames; ++i) /*step through frame array and check for matches*/
    {
        if(frames[i] == page)
        {
            hit = 1;
            break;
        }
    }
    return hit;
}


void pr_fifo(int pages[])
{
    //implement FIFO to count pf_fifo

    //loop through page refs
        // for each page requested in page_refs
        //see if you already have that page in FRAME
            //if so, then continue on to the next page request
            //if not then it IS a page fault (update pf_fifo)
                //see if you have empty space in FRAME
                    //if so,add that page to FRAME
                    //if not, find oldest page in FRAME <-- queue like implementation
                        //replace oldest page with requested page
    
    int i, j, k;
    int fifo_frames[total_frames];
    int idx = 0;

    for(i = 0; i < total_frames; ++i)
    {
        fifo_frames[i] = -1; /* intializing frames to -1 to deferntiate from other data*/
    }

    for(i = 0; i < total_refs; ++i) /*check if page is already in frame */
    {
        if(!hit(fifo_frames, pages[i]) && idx < total_frames)
        {
            fifo_frames[idx] = pages[i];
            idx++;
            pf_fifo++;
        }
        else if(!hit(fifo_frames, pages[i]))
        {
            /* if page fault */
            for(j = 0; j < total_frames-1; ++j)
            {
                fifo_frames[j] = fifo_frames[j+1];
                fifo_frames[j] = pages[i];
                pf_fifo++;
            }
        }
    }
}

void pr_lru(int pages[])
{
    //must consider and decide which page has not been used for the longest time
    //implement this in stack, as discussed in slides

    int i, j, k;
    int lru_frames[total_frames];
    int least[total_frames];
    int idx = 0;

    for(i = 0; i < total_frames; ++i)
    {
        lru_frames[i] = -1; /* intializing frames to # < 0 */
    }

    for(i = 0; i < total_refs; ++i) /*check if page is already in frame */
    {
        if(!hit(lru_frames, pages[i]) && idx < total_frames)
        {
            lru_frames[idx] = pages[i];
            idx++;
            pf_fifo++;
        }
        else if(!hit(lru_frames, pages[i]))
        {
            /* if page fault */
            for(j = 0; j < total_frames; ++j)/*cycle through all frames */
            {
                int page_num = lru_frames[i];
                int located = 0;
                for(k=i-1; k >= 0; k--) /*loop through page ref back to start*/
                {
                    if(page_num == pages[k])/* if page # found*/
                    {
                        least[j] = k; /* save index of most recent */
                        located = 1;  /* page located, break loop */
                        break;
                    }
                    else 
                    located = 0;
                }
                if(!located)
                {
                    least[j] = -9999; /*fake index for unfound page*/
                }
            }
            int min = 9999;
            int replace_idx;
            for(j = 0; j < total_frames; j++)
            {
                if(least[j] < min)
                {
                    min = least[j];
                    replace_idx = j;
                }
            }
            lru_frames[replace_idx] = pages[i];
            pf_lru++;
        }
    } 
}

void pr_optimal(int pages[])
{
    //must consider and decide which page will be referenced farthest away into the future

    int i, j, k;
    int opt_frames[total_frames];
    int distance[total_frames];
    int idx = 0;

    for(i = 0; i < total_frames; ++i)
    {
        opt_frames[i] = -1; /* intializing frames to # < 0 */
    }

    for(i = 0; i < total_refs; ++i) /*check if page is already in frame */
    {
        if(!hit(opt_frames, pages[i]) && idx < total_frames)
        {
            opt_frames[idx] = pages[i];
            idx++;
            pf_opt++;
        }
        else if(!hit(opt_frames, pages[i]))
        {
            /* if page fault */
            for(j = 0; j < total_frames; ++j)/*cycle through all frames */
            {
                int page_num = opt_frames[i];
                int located = 0;
                for(k=i; k < total_frames; k++) /*loop through page ref back to start*/
                {
                    if(page_num == pages[k])/* if page # found*/
                    {
                        distance[j] = k; /* save index of most recent */
                        located = 1;  /* page located, break loop */
                        break;
                    }
                    else 
                    located = 0;
                }
                if(!located)
                {
                    distance[j] = -9999; /*fake index for unfound page*/
                }
            }
            int max = 9999; /*max distance set */
            int replace_idx;
            for(j = 0; j < total_frames; j++)
            {
                if(distance[j] > max)
                {
                    max = distance[j];
                    replace_idx = j;
                }
            }
            opt_frames[replace_idx] = pages[i];
            pf_opt++;
        }
    } 
}

int main(int argc, char** argv)
{  
    total_frames = atoi(argv[1]);
    if(total_frames < 1 || total_frames > 7)
    {
        printf("Total frames must be in range from [1, 7]\n");
        return -1;
    }

    //FRAME = (int*)malloc(sizeof(int)* total_frames);

    FILE* fp = fopen("input.txt", "rt");
    if (fp == NULL) return -1;

    fscanf(fp, "%d", &total_refs);

    page_refs = (int*)malloc(sizeof(int)* total_refs);

    for(int i = 0; i <total_refs; ++i)
    {
        fscanf(fp, "%d", &(page_refs[i]));
    }

    fclose(fp);

    //checking input
    /*
    printf("total: %d\n", total_refs);
    for(int i = 0; i < total_refs; ++i)
    {
        printf("%d ", page_refs[i]);
    }
    printf("\n"); 
    */
    
    //Run FIFO
    pr_fifo(page_refs);

    //Run LRU
    pr_lru(page_refs);

    //Run Optimal
    pr_optimal(page_refs);

    //Report results
    fp = fopen("output.txt", "wt");

    fprintf(fp, "FIFO: %d\n", pf_fifo);
    fprintf(fp, "LRU: %d\n", pf_lru);
    fprintf(fp, "Optimal: %d\n", pf_opt);

    fclose(fp);

    //free(page_refs);
    //free(FRAME);

    return 0;
}