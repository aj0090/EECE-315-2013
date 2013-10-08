#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>


#define NUM_COL_THREADS 1
#define NUM_ROW_THREADS 1
#define NUM_BLOCK_THREADS 1
#define SUDOKU_DEPTH 9


char sudokustring[81] = "123567894456189237789234156214356789365798412897412365532641978648973521971825643";


void *isValid(void *);
void defaultSudokuArray(int **, char *);

typedef struct NumberArray{
	int **elements;
	int elements_count;
	int isValid;
} NumberArray;



int main(int argc, char *argv[])
{

	int **sudokuarray;

	printf("Program start.\n");
	generateSudokuArray(sudokuarray, sudokustring);


	pthread_t columnThreads[NUM_COL_THREADS];
	pthread_t rowThreads[NUM_ROW_THREADS];
	pthread_t blockThreads[NUM_BLOCK_THREADS];

	
	NumberArray column_NumberArrays[NUM_COL_THREADS];
	NumberArray row_NumberArrays[NUM_ROW_THREADS];
	NumberArray block_NumberArrays[NUM_BLOCK_THREADS];

	int col_i, col_j;
	int row_i, row_j;
	int block_i, block_j;

	for(col_i = 0; col_i < NUM_COL_THREADS; col_i++)
	{
		column_NumberArrays[col_i]->isValid = true;
		int numItems;
	}

	

	free(sudokuarray);

}




void *isValid(void *arg)
{
	int i, j;
	NumberArray *testNumberArray = arg;

	for(i = 0; i < testNumberArray->elements_count; i++)
	{
		int numbersFound[SUDOKU_DEPTH] = {0};
		for(j = 0; j < SUDOKU_DEPTH; j++)
		{
			if(testNumberArray->elements[i][j] == 1){
				testNumberArray->isValid = 0;
				break;
			}
			else{
				testNumberArray->elements[i][j] = 1;
			}
		}
		if(testNumberArray->isValid == 0)
			break;
	}
}

void generateSudokuArray(int **sudokuarray, char *sudokustring)
{
	int i, j;

	sudokuarray = malloc(sizeof(int*) * 9);
	for(i = 0; i < 9; i++)
		sudokuarray[i] = malloc(sizeof(int) * 9);

	for(i = 0; i < 9; i++){
		if(i == 3 || i == 6)
			printf("---------------------\n");
		for(j = 0; j < 9; j++){
			// Correct ASCII to integer with 0x30
			sudokuarray[i][j] = sudokustring[i * 9 + j] - 0x30;	
			printf("%d ", sudokuarray[i][j]);
			//printf("%d ", sudokustring[i * 9 + j] - 0x30);
			if(j == 2 || j == 5)
				printf("| ");	
		}	
		//printf("%d", sudokuarray[0][0]);
		printf("\n");
	}
}



