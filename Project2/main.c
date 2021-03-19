#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

// Array to track invalid threads, a value of 1 at any index means the solution is invalid
int invalid = 0;

// Struct that stores the data to be passed to threads
typedef struct {
	int row;
	int column;		
} parameters;

// Initialize Sudoku puzzle to be solved
int sudoku[9][9];

// Column validity check function
void *isColValid(void* param) {
	// Confirm that parameters indicate a valid col subsection
	parameters *params = (parameters*) param;
	int row = params->row;
	int col = params->column;		
	if (row != 0 || col > 8) {
		fprintf(stderr, "Invalid row or column for col subsection! row=%d, col=%d\n", row, col);
		pthread_exit(NULL);
	}

	// Check if numbers 1-9 only appear once in the column
	int validityArray[9] = {0};
	int i;	
	for (i = 0; i < 9; i++) {
		int num = sudoku[i][col];
		if (num < 1 || num > 9 || validityArray[num - 1] == 1) {
			invalid = 1;	// column is invalid
			pthread_exit(NULL);
		} else {
			validityArray[num - 1] = 1;		
		}
	}
	pthread_exit(NULL);
}

// Check validity of all rows in one thread
void *areColsValid(void* param) {
	int i, j, k, p;
	int tempRow[9];
	for (i = 0; i < 9; i++) {
		for (j = 0; j < 9; j++) {
			tempRow[j] = sudoku[j][i];
		}
		for (k = 0; k < 9; k++) {
			for (p = k + 1; p < 9; p++) {
				if(tempRow[k] == tempRow[p]) {
					invalid = 1;	// row is invalid
					pthread_exit(NULL);
				}
			}
		}
	}
}

// Row validity check function
void *isRowValid(void* param) {
	// Confirm that parameters indicate a valid row subsection
	parameters *params = (parameters*) param;
	int row = params->row;
	int col = params->column;		
	if (col != 0 || row > 8) {
		fprintf(stderr, "Invalid row or column for row subsection! row=%d, col=%d\n", row, col);
		pthread_exit(NULL);
	}

	// Check if numbers 1-9 only appear once in the row
	int validityArray[9] = {0};
	int i;
	for (i = 0; i < 9; i++) {
		// If the corresponding index for the number is set to 1, and the number is encountered again,
		// the valid array will not be updated and the thread will exit.
		int num = sudoku[row][i];
		if (num < 1 || num > 9 || validityArray[num - 1] == 1) {
			invalid = 1;	// row is invalid
			pthread_exit(NULL);
		} else {
			validityArray[num - 1] = 1;		
		}
	}

	pthread_exit(NULL);
}

// Check validity of all rows in one thread
void *areRowsValid(void* param) {
	int i, j, k, p;
	int tempRow[9];
	for (i = 0; i < 9; i++) {
		for (j = 0; j < 9; j++) {
			tempRow[j] = sudoku[i][j];
		}
		for (k = 0; k < 9; k++) {
			for (p = k + 1; p < 9; p++) {
				if(tempRow[k] == tempRow[p]) {
					invalid = 1;	// row is invalid
					pthread_exit(NULL);
				}
			}
		}
	}
}

// Grid validity check function
void *is3x3Valid(void* param) {
	// Confirm that parameters indicate a valid 3x3 subsection
	parameters *params = (parameters*) param;
	int row = params->row;
	int col = params->column;		
	if (row > 6 || row % 3 != 0 || col > 6 || col % 3 != 0) {
		fprintf(stderr, "Invalid row or column for subsection! row=%d, col=%d\n", row, col);
		pthread_exit(NULL);
	}
	int validityArray[9] = {0};
	int i, j;
	for (i = row; i < row + 3; i++) {
		for (j = col; j < col + 3; j++) {
			int num = sudoku[i][j];
			if (num < 1 || num > 9 || validityArray[num - 1] == 1) {
				invalid = 1;	// grid is invalid
				pthread_exit(NULL);
			} else {
				validityArray[num - 1] = 1;		
			}
		}
	}
	pthread_exit(NULL);
}

int main(int argc, char** argv) {	
	int option = atoi(argv[1]);
	int num_threads;

	printf("BOARD STATE IN input.txt:\n");

    FILE *input;
    input = fopen("input.txt","r");
	int i, j;

    /* Read and assign sudoku board from input.txt */
    for (i=0;i<9;i++)
    {
        for (j=0;j<9;j++)
        {
            fscanf(input, "%1d", &sudoku[i][j]);
            printf("%d ", sudoku[i][j]);
        }
        printf("\n");
    }

	clock_t t;
	t = clock();

	if(option == 1) {
		num_threads = 11;
		//printf("11 threads to be used...\n");
	} else if(option == 2) {
		num_threads = 27;
		//printf("27 threads to be used...\n");
	} else if(option == 3) {
		num_threads = 27;
		//printf("Processes to be used...\n");
	}

	pthread_t threads[num_threads];
	

	int threadIndex = 0;	
	if (num_threads == 27) {
		for (i = 0; i < 9; i++) {
			for (j = 0; j < 9; j++) {						
				if (i%3 == 0 && j%3 == 0) {
					parameters *data = (parameters *) malloc(sizeof(parameters));	
					data->row = i;		
					data->column = j;
					pthread_create(&threads[threadIndex++], NULL, is3x3Valid, data); // 3x3 subsection threads
				}
				if (i == 0) {
					parameters *columnData = (parameters *) malloc(sizeof(parameters));	
					columnData->row = i;		
					columnData->column = j;
					pthread_create(&threads[threadIndex++], NULL, isColValid, columnData);	// column threads
				}
				if (j == 0) {
					parameters *rowData = (parameters *) malloc(sizeof(parameters));	
					rowData->row = i;		
					rowData->column = j;
					pthread_create(&threads[threadIndex++], NULL, isRowValid, rowData); // row threads
				}
			}
		}
	}
	else if (num_threads == 11) {
		// 1 thread for row validity
		pthread_create(&threads[threadIndex++], NULL, areRowsValid, NULL); // row thread
		// 1 thread for col validity
		pthread_create(&threads[threadIndex++], NULL, areColsValid, NULL);	// column threads

		for (i = 0; i < 9; i++) {
			for (j = 0; j < 9; j++) {						
				if (i%3 == 0 && j%3 == 0) {
					parameters *data = (parameters *) malloc(sizeof(parameters));	
					data->row = i;		
					data->column = j;
					pthread_create(&threads[threadIndex++], NULL, is3x3Valid, data); // 3x3 subsection threads
				}
			}
		}
	}
	for (i = 0; i < num_threads; i++) {
		pthread_join(threads[i], NULL);			// Wait for all threads to finish
	}

	t = clock() - t;
	double total_time = ((double)t/CLOCKS_PER_SEC);
	

	if (invalid == 1) {
		printf("SOLUTION: NO (%f seconds)\n", total_time);
		return 0;
	}

	printf("SOLUTION: YES (%f seconds)\n", total_time);
	return 0;
}