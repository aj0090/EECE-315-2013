#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <math.h>


#define NUM_COL_THREADS 3
#define NUM_ROW_THREADS 3
#define NUM_BLOCK_THREADS 3


#define SUDOKU_DEPTH 9


int testSudokuString(char *);


typedef struct NumberArray{
	int **elements;
	int array_count;
	int isValid;
} NumberArray;

int testNumberArrays(NumberArray *, NumberArray *, NumberArray *);
void *isValid(void *);
int *copyOfRow(int **, int);
int *copyOfColumn(int **, int);
int *copyOfBlock(int **, int);

void initializeArrays(NumberArray *, NumberArray *, NumberArray *);
void freeNumberArrays(NumberArray *, NumberArray *, NumberArray *);

int **generateSudokuArray(int **, char *);

int max(int, int);




int main(int argc, char *argv[])
{

	char correct_sudokustring[81] = "123567894456189237789234156214356789365798412897412365532641978648973521971825643";

	char incorrect_sudokustring[81] = "103567894456189237789234156214356789365798412897412365532641978648973521977895643";


	testSudokuString(correct_sudokustring);
}


int testSudokuString(char *sudokustring)
{
	int **sudokuarray;

	printf("Program start.\n");
	sudokuarray = generateSudokuArray(sudokuarray, sudokustring);

	NumberArray row_NumberArrays[NUM_ROW_THREADS];
	NumberArray column_NumberArrays[NUM_COL_THREADS];
	NumberArray block_NumberArrays[NUM_BLOCK_THREADS];
	initializeArrays(row_NumberArrays, column_NumberArrays, block_NumberArrays);



	int i, row_cursor, col_cursor, block_cursor;

	i = 0;

	while(i < 9)
	{
		row_cursor = i % NUM_ROW_THREADS;
		col_cursor = i % NUM_COL_THREADS;
		block_cursor = i % NUM_BLOCK_THREADS;

		printf("i : %d. row_cursor: %d.\n", i, row_cursor);
		row_NumberArrays[row_cursor].elements[row_NumberArrays[row_cursor].array_count++] = copyOfRow(sudokuarray, i);
		column_NumberArrays[col_cursor].elements[column_NumberArrays[col_cursor].array_count++] = copyOfColumn(sudokuarray, i);
		block_NumberArrays[block_cursor].elements[block_NumberArrays[block_cursor].array_count++] = copyOfBlock(sudokuarray, i);
	
		i++;

		printf("array_count: %d\n\n", row_NumberArrays[row_cursor].array_count);
	}

	testNumberArrays(row_NumberArrays, column_NumberArrays, block_NumberArrays);



	freeNumberArrays(row_NumberArrays, column_NumberArrays, block_NumberArrays);
	free(sudokuarray);
}


int testNumberArrays(NumberArray *row_numberarrays, NumberArray *column_numberarrays, NumberArray *block_numberarrays)
{
	pthread_t rowThreads[NUM_ROW_THREADS];
	pthread_t columnThreads[NUM_COL_THREADS];
	pthread_t blockThreads[NUM_BLOCK_THREADS];
	int i, max_threads;

	max_threads = max(max(NUM_ROW_THREADS, NUM_COL_THREADS), NUM_BLOCK_THREADS);
	// Create row, column, and block threads
	for(i = 0; i < max_threads; i++)
	{
		if(i < NUM_ROW_THREADS)
			if(pthread_create(&rowThreads[i], NULL, isValid, (&row_numberarrays[i]) ))
				printf("Couldn't create thread for row %d.\n", i);

		if(i < NUM_COL_THREADS)
			if(pthread_create(&columnThreads[i], NULL, isValid, (&column_numberarrays[i]) ))
				printf("Couldn't create thread for column %d.\n", i);

		if(i < NUM_BLOCK_THREADS)
			if(pthread_create(&blockThreads[i], NULL, isValid, (&block_numberarrays[i]) ))
				printf("Couldn't create thread for column %d.\n", i);
		
	}


	// Start execution on all of the threads
	

	for(i = 0; i < max_threads; i++)
	{
		if(i < NUM_ROW_THREADS)
			if(pthread_join(rowThreads[i], NULL))
				printf("Couldn't create thread for row %d.\n", i);

		if(i < NUM_COL_THREADS)
			if(pthread_join(columnThreads[i], NULL))
				printf("Couldn't create thread for column %d.\n", i);

		if(i < NUM_BLOCK_THREADS)
			if(pthread_join(blockThreads[i], NULL))
				printf("Couldn't create thread for column %d.\n", i);
		
	}
/*
	for(i = 0; i < NUM_ROW_THREADS && i < 9; i++)
	{
		if(pthread_join(rowThreads[i], NULL))
			printf("Couldn't join thread for row %d.\n",i);
	}
	for(i = 0; i < NUM_COL_THREADS && i < 9; i++)
	{
		if(pthread_join(columnThreads[i], NULL))
			printf("Couldn't join thread for column %d.\n",i);
	}
*/


}

void *isValid(void *arg)
{
	int i, j;
	NumberArray *testNumberArray = arg;
	

	for(i = 0; i < testNumberArray->array_count; i++)
	{
		int numbersFound[SUDOKU_DEPTH] = {0};
		for(j = 0; j < SUDOKU_DEPTH; j++)
		{
			//printf("number %d: %d\n", j, testNumberArray->elements[i][j]);
			if(numbersFound[testNumberArray->elements[i][j] - 1] == 1 && testNumberArray->elements[i][j] >= 1 && testNumberArray->elements[i][j] <= 9)
			{
				testNumberArray->isValid = 0;
				printf("Array not valid!\n");
				break;
			}
			else{
				numbersFound[testNumberArray->elements[i][j] - 1] = 1;
			}
		}
		if(testNumberArray->isValid == 0)
			break;
	}
	printf("Returning %d\n", testNumberArray->isValid);

}


int *copyOfRow(int **sudokuarray, int row)
{
	int *temp = malloc(sizeof(int)*9);
	int i;

	for(i = 0; i < 9; i++)
	{
		temp[i] = sudokuarray[row][i];
		printf("%d", temp[i]);
	}
	printf("\n");
	return temp;
}

int *copyOfColumn(int **sudokuarray, int column)
{
	int *temp = malloc(sizeof(int)*9);
	int i;

	for(i = 0; i < 9; i++)
	{
		temp[i] = sudokuarray[i][column];
		printf("%d\n", temp[i]);
	}
	//printf("\n");
	return temp;
}

// 1 2 3
// 4 5 6
// 7 8 9
int *copyOfBlock(int **sudokuarray, int blocknumber)
{
	int *temp = malloc(sizeof(int)*9);
	int vertical_pos = blocknumber / 3;
	int horizontal_pos = blocknumber % 3;

	int i, j, k;

	i = 0;
	for(j = vertical_pos; j < vertical_pos + 3; j++)
	{
		for(k = horizontal_pos; k < horizontal_pos + 3; k++)
		{
			temp[i] = sudokuarray[j][k];
			printf("%d", temp[i]);
			i++;
		}
		printf("\n");
	}
	printf("\n");
	return temp;
}



void initializeArrays(NumberArray *row_numberarrays, NumberArray *column_numberarrays, NumberArray *block_numberarrays)
{
	int i;

	for(i = 0; i < NUM_ROW_THREADS; i++)
	{
		row_numberarrays[i].array_count = 0;
		row_numberarrays[i].isValid = 1;
		row_numberarrays[i].elements = malloc(sizeof(int *) * (SUDOKU_DEPTH / NUM_ROW_THREADS + 1));
	}

	for(i = 0; i < NUM_COL_THREADS; i++)
	{
		column_numberarrays[i].array_count = 0;
		column_numberarrays[i].isValid = 1;
		column_numberarrays[i].elements = malloc(sizeof(int *) * (SUDOKU_DEPTH / NUM_COL_THREADS + 1));
	}

	for(i = 0; i < NUM_BLOCK_THREADS; i++)
	{
		block_numberarrays[i].array_count = 0;
		block_numberarrays[i].isValid = 1;
		block_numberarrays[i].elements = malloc(sizeof(int *) * (SUDOKU_DEPTH / NUM_BLOCK_THREADS + 1));
	}

}



void freeNumberArrays(NumberArray *row_numberarrays, NumberArray *column_numberarrays, NumberArray *block_numberarrays)
{
	int i;
	for(i = 0; i < NUM_ROW_THREADS; i++)
	{
		free(row_numberarrays[i].elements);
	}
	for(i = 0; i < NUM_COL_THREADS; i++)
	{
		free(column_numberarrays[i].elements);
	}
	for(i = 0; i < NUM_BLOCK_THREADS; i++)
	{
		free(block_numberarrays[i].elements);
	}
}



int **generateSudokuArray(int **sudokuarray, char *sudokustring)
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
	return sudokuarray;
}

int max(int num1, int num2)
{
	if(num1 >= num2)
		return num1;
	else
		return num2;
}



