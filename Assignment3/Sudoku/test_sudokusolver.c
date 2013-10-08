#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>


#define NUM_COL_THREADS 1
#define NUM_ROW_THREADS 1
#define NUM_BLOCK_THREADS 1
#define SUDOKU_DEPTH 9


char sudokuString[81] = "123567894456189237789234156214356789365798412897412365532641978648973521971825643";


void *isValid(void *);
int **defaultSudokuArray();

typedef struct NumberArray{
	int **elements;
	int elements_count;
	int isValid;
} NumberArray;



int main(int argc, char *argv[])
{
	pthread_t columnThreads[NUM_COL_THREADS];
	pthread_t rowThreads[NUM_ROW_THREADS];
	pthread_t blockThreads[NUM_BLOCK_THREADS];

	
	NumberArray column_NumberArrays[NUM_COL_THREADS];
	NumberArray row_NumberArrays[NUM_ROW_THREADS];
	NumberArray block_NumberArrays[NUM_BLOCK_THREADS];

	int sudokuarray[9][9];

	generateSudokuArray(sudokuarray);

}




void *isValid(void *arg)
{
	NumberArray *testNumberArray = arg;

	for(int i = 0; i < testNumberArray; i++)
	{
		int numbersFound[SUDOKU_DEPTH] = {0};
		for(int j = 0; j < SUDOKU_DEPTH; j++)
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

void generateSudokuArray(int ** )
{
	
}



