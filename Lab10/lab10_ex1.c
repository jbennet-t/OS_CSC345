/*
Jordan Sinoway
CSC 345-02
lab10_ex1
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

int max_cyl;
int max_req;
int cur_loc;
int* reqs;

int fcfs()
{
	int total = 0;

	for(int i = 0; i < max_req; ++i)
	{
		total += abs(reqs[i] - cur_loc);
		cur_loc = reqs[i];
	}
	return total;
}

int scan()
{
	int total = 0;
	bool isLTR;
	//determine head direction
	if (cur_loc < reqs[0])
		isLTR = true;
	else
		isLTR = false;

	//sort request
	int min = reqs[0];
	int max = reqs[0];

	for(int i = 0; i < max_req; ++i)
	{
		if(reqs[i] < min)
		{
			min = reqs[i];
		}
		if(reqs[i] > max)
		{
			max = reqs[i];
		}

	}

	if (cur_loc < max && cur_loc > min)
	{	
		if(isLTR)
		{
			total = (max_cyl - cur_loc) + (max_cyl - min);
		}
		else
		{
			total = (cur_loc + max);
		}
	}
	else if(cur_loc <= min)
	{
		if(isLTR)
		{
			total = (max - cur_loc);
		}
		else
		{
			total =	(max - cur_loc);
		}
	}
	else if(cur_loc >= max)
	{
		if(isLTR)
		{	
			total = (max_cyl - cur_loc) + (max_cyl - min);
		}
		else
		{
			total = (cur_loc - min);
		}
	}


	return total;
}

int cscan()
{
	int total = 0;
	int min = reqs[0];
	int max = reqs[0];
	int max_b_cur = reqs[0];

	for(int i = 1; i < max_req; ++i)
	{
		if(reqs[i] < min)
		{
			min = reqs[i];
		}	
		if(reqs[i] > max)
		{
			max = reqs[i];
		}
		if(reqs[i] > max_b_cur && reqs[i] < cur_loc)
		{
			max_b_cur = reqs[i];
		}
	}

	if(cur_loc < max && cur_loc > min)
	{
		total = (max_cyl - cur_loc) + max_b_cur;
	}
	else if(cur_loc <= min)
	{
		total = (max - cur_loc);
	}
	else if(cur_loc >= max)
	{
		total = (max_cyl - cur_loc) + max;	
	}
	return total;
}

int main(int argc, char** argv)
{
	FILE* fp = fopen("input.txt", "rt");

	fscanf(fp, "%d", &max_cyl);
	fscanf(fp, "%d", &max_req);

	reqs = (int*)malloc(sizeof(int) * max_req);

	for(int i = 0; i < max_req; ++i)
	{
		fscanf(fp, "%d", &reqs[i]);
	}

	fscanf(fp, "%d", &cur_loc);

	fclose(fp);


	fp = fopen("output.txt", "wt");

	fprintf(fp, "%d\n", fcfs());
	fprintf(fp, "%d\n", scan());
	fprintf(fp, "%d\n", cscan());


	free(reqs);

	return 0;
}
