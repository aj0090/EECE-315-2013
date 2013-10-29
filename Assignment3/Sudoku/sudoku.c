#include "sudoku.h"

int main(int argc, char *argv[])
{
	srand(time(NULL));

	char correct_sudokustring[] = "123567894456189237789234156214356789365798412897412365532641978648973521971825643";

	printf("Result: %s\n", (testSudokuString(correct_sudokustring)) ? "Correct" : "Incorrect" );

	char incorrect_sudokustring[] = "103567894456189237789234156214356789365798412897412365532641978648973521977895643";

	printf("Result: %s\n", (testSudokuString(incorrect_sudokustring)) ? "Correct" : "Incorrect" );

	char sudokuarray[81];

	printf("Input a Sudoku Solution, and I'll test for validity\n");
	scanf("%s", sudokuarray);

	printf("Result: %s\n", (testSudokuString(sudokuarray)) ? "Correct" : "Incorrect" );

	printf("Generate a sudoku puzzle! What difficulty?\n");
	printf("0 = BEGINNER, 1 = EASY, 2 = MEDIUM, 3 = HARD, 4 = VERY HARD\n");
	int difficulty;
	int cellcount;

	scanf("%d", &difficulty);

	if(difficulty == 0) {
		cellcount = BEGINNER;
	} else if (difficulty == 1) {
		cellcount = EASY;
	} else if (difficulty == 2) {
		cellcount = MEDIUM;
	} else if (difficulty == 3) {
		cellcount = HARD;
	} else {
		cellcount = VERY_HARD;
	};
	generateSudokuPuzzle(cellcount);
}

// REQUIRES: sudokustring only comprises of characters 0 <= x <= 9
// MODIFIES: none
// EFFECTS: Tests the validity of a sudoku string. If the sudokustring is a valid sudoku solution, returns 1. Else, returns 0.
int testSudokuString(char *sudokustring)
{
	int **sudokuarray;

	// Return -1 if the length of the input string isn't exactly 81.
	if(strlen(sudokustring) != 81){
		printf("Invalid input size %d.\n", strlen(sudokustring));
		return -1;
	}

	// Decode the string into a 9x9 array
	sudokuarray = decodeSudokuString(sudokustring);
	printSudokuPuzzle(sudokuarray);


	// Declare/Initialize the structs based on the number of threads for each one
	NumberArray row_NumberArrays[NUM_ROW_THREADS];
	NumberArray column_NumberArrays[NUM_COL_THREADS];
	NumberArray block_NumberArrays[NUM_BLOCK_THREADS];
	initializeArrays(row_NumberArrays, column_NumberArrays, block_NumberArrays);

	int i, row_cursor, col_cursor, block_cursor;

	i = 0;

	// Allocate 9-element arrays to each of the structs, with rollover cursors that
	// depend on the number of structs available
	while(i < 9)
	{
		row_cursor = i % NUM_ROW_THREADS;
		col_cursor = i % NUM_COL_THREADS;
		block_cursor = i % NUM_BLOCK_THREADS;

		if(VERBOSE) printf("i : %d. row_cursor: %d.\n", i, row_cursor);

		// Create a copy of the row, column, block, and allocate the copy to a portion in the struct for the current cursor position for row, column and block
		row_NumberArrays[row_cursor].elements[row_NumberArrays[row_cursor].array_count++] = copyOfRow(sudokuarray, i);
		column_NumberArrays[col_cursor].elements[column_NumberArrays[col_cursor].array_count++] = copyOfColumn(sudokuarray, i);
		block_NumberArrays[block_cursor].elements[block_NumberArrays[block_cursor].array_count++] = copyOfBlock(sudokuarray, i);

		i++;

		if(VERBOSE) printf("array_count: %d\n\n", row_NumberArrays[row_cursor].array_count);
	}

	// Allocate the result to the 'result' variable
	int result = testNumberArrays(row_NumberArrays, column_NumberArrays, block_NumberArrays);


	// Free the dynamic memory
	freeNumberArrays(row_NumberArrays, column_NumberArrays, block_NumberArrays);
	for(i = 0; i < 9; i++)
		free(sudokuarray[i]);
	free(sudokuarray);

	return result;
}

//EFFECTS: Outputs a Sudoku Puzzle with "count" values filled in
void generateSudokuPuzzle(int count) {
	char sudokustring[] = "123567894456189237789234156214356789365798412897412365532641978648973521971825643";
	int usedArray[9][9];
	int countArray[9] = {0,0,0,0,0,0,0,0,0} ;
	int i, j, cursor1, cursor2, counter;
	int **sudokuarray;

	memset(usedArray, 0, 9 * sizeof(usedArray[0]));

	sudokuarray = decodeSudokuString(sudokustring);
	if (VERBOSE) printSudokuPuzzle(sudokuarray);

	for (i = 0; i < (rand() % count); i ++) {

		if (VERBOSE) printf("Swapping rows.\n");
		swapRows(&sudokuarray);
		if (VERBOSE) printSudokuPuzzle(sudokuarray);
		if (VERBOSE) printf("Rotating array.\n");
		sudokuarray = rotateArray(sudokuarray);
		if (VERBOSE) printSudokuPuzzle(sudokuarray);
	}

	for(i=0; i< (81-count); i++) {

		do {
			cursor1 = rand() % 9;
			cursor2 = rand() % 9;
		} while (usedArray[cursor1][cursor2]);

		usedArray[cursor1][cursor2] = 1;

		if (countArray[sudokuarray[cursor1][cursor2]-1] >= 8) {
			if (VERBOSE) printf("%d has already been taken away 8 times\n", sudokuarray[cursor1][cursor2]);
			i--;
		}
		else {
			countArray[sudokuarray[cursor1][cursor2]-1] = countArray[sudokuarray[cursor1][cursor2]-1] + 1;
			if (VERBOSE) printf("%d has already been taken away %d times\n", sudokuarray[cursor1][cursor2], countArray[sudokuarray[cursor1][cursor2]-1]);
			sudokuarray[cursor1][cursor2]=0;
		}
	}

	printSudokuPuzzle(sudokuarray);

	for (i = 0, counter = 0; i < 9; i++)
		for (j = 0; j < 9; j++, counter++) {
			sudokustring[counter] = (char) (((int) '0') + sudokuarray[i][j]);
	}

	printf("Testing if sudoku puzzle is correct\n");
	printf("Result: %s\n", (testSudokuString(sudokustring)) ? "Correct" : "Incorrect" );

	for(i = 0; i < 9; i++)
		free(sudokuarray[i]);
	free(sudokuarray);
}

// REQUIRES: A 9x9 2D integer array as input
// MODIFIES: array
// EFFECTS: Frees up array, and returns a array that is rotated 90 degrees
int **rotateArray(int **array)
{
	int **rotatedarray, i, j;

	rotatedarray = malloc(sizeof(int*) * 9);
	for(i = 0; i < 9; i++)
		rotatedarray[i] = malloc(sizeof(int) * 9);

	for(i=0; i<9; i++) {
   		for(j=0; j<9; j++) {
        		rotatedarray[i][j] = array[9-1-j][i];
    		}
	}

	for(i = 0; i < 9; i++)
		free(array[i]);
	free(array);

	return rotatedarray;
}

// REQUIRES: A 9x9 2D integer array as input
// MODIFIES: array
// EFFECTS: Swaps a pair of rows within the 3x3 block
void swapRows(int ***array)
{
	int * tempLine, cursor1, cursor2, i;

	for(i = 0; i < 9; i=i+3) {
		cursor1 = rand() % 3;
		cursor2 = rand() % 3;
		tempLine = (*array)[i+cursor1];
		(*array)[i+cursor1] = (*array)[i+cursor2];
		(*array)[i+cursor2] = tempLine;
	}
}

// REQUIRES: Fully initialized/valid NumberArrays, defined NUM_ROW_THREADS, NUM_COL_THREADS, NUM_BLOCK_THREADS
// MODIFIES: row_numberarrays, column_numberarrays, block_numberarrays
// EFFECTS: Creates threads to run every NumberArray struct on the isValid() function. After the structs isTested variables == 1, if any of the isValid variables == 0, return 0. Else, return 1.
int testNumberArrays(NumberArray *row_numberarrays, NumberArray *column_numberarrays, NumberArray *block_numberarrays)
{
	pthread_t rowThreads[NUM_ROW_THREADS];
	pthread_t columnThreads[NUM_COL_THREADS];
	pthread_t blockThreads[NUM_BLOCK_THREADS];
	int i, max_threads;

	// Use max_threads variable to avoid using unnecessary 'for' loops
	max_threads = max(max(NUM_ROW_THREADS, NUM_COL_THREADS), NUM_BLOCK_THREADS);


	// Create row, column, and block threads
	for(i = 0; i < max_threads; i++)
	{
		if(i < NUM_ROW_THREADS)
			if(pthread_create(&rowThreads[i], NULL, isValid, (&row_numberarrays[i]) ))
				if(VERBOSE) printf("Couldn't create thread for row %d.\n", i);

		if(i < NUM_COL_THREADS)
			if(pthread_create(&columnThreads[i], NULL, isValid, (&column_numberarrays[i]) ))
				if(VERBOSE) printf("Couldn't create thread for column %d.\n", i);

		if(i < NUM_BLOCK_THREADS)
			if(pthread_create(&blockThreads[i], NULL, isValid, (&block_numberarrays[i]) ))
				if(VERBOSE) printf("Couldn't create thread for column %d.\n", i);

	}

	// Start execution on all of the threads
	for(i = 0; i < max_threads; i++)
	{
		if(i < NUM_ROW_THREADS)
			if(pthread_join(rowThreads[i], NULL))
				if(VERBOSE) printf("Couldn't create thread for row %d.\n", i);

		if(i < NUM_COL_THREADS)
			if(pthread_join(columnThreads[i], NULL))
				if(VERBOSE) printf("Couldn't create thread for column %d.\n", i);

		if(i < NUM_BLOCK_THREADS)
			if(pthread_join(blockThreads[i], NULL))
				if(VERBOSE) printf("Couldn't create thread for column %d.\n", i);
	}

	// Test all the structs isTested bits incrementally until they all == 1.
	// If any fo the isValid members == 0, break the loop and set isvalid to 0.
	int isValid = 1;
	for(i = 0; i < max_threads; i++)
	{
		if(i < NUM_ROW_THREADS){
			// Wait until the struct is tested
			while(row_numberarrays[i].isTested == 0)
			;
			if(row_numberarrays[i].isValid == 0){
				isValid = 0;
				break;
			}

		}
		if(i < NUM_COL_THREADS){
			while(column_numberarrays[i].isTested == 0)
			;
			if(column_numberarrays[i].isValid == 0){
				isValid = 0;
				break;
			}
		}
		if(i < NUM_BLOCK_THREADS){
			while(block_numberarrays[i].isTested == 0)
			;
			if(block_numberarrays[i].isValid == 0){
				isValid = 0;
				break;
			}
		}
	}

	if(VERBOSE) printf("------------%d\n", isValid);

	return isValid;
}


// REQUIRES: Fully initialized, valid NumberArray struct
// MODIFIES: NumberArray struct
// EFFECTS: If duplicates exist within any of the 9-element arrays,
// set the struct variable isValid to 0. At the end of the function,
// set the struct variable isTested to 1.
void *isValid(void *arg)
{
	int i, j;
	NumberArray *testNumberArray = arg;

	// Iterate through each 9-element array
	for(i = 0; i < testNumberArray->array_count; i++)
	{
		// Keep track of 'seen' elements within the 9-element array
		int numbersFound[SUDOKU_DEPTH] = {0,0,0,0,0,0,0,0,0};
		for(j = 0; j < SUDOKU_DEPTH; j++)
		{
			// If element has been seen before in the 9-element array
			if((testNumberArray->elements[i][j])
				&& numbersFound[testNumberArray->elements[i][j] - 1] == 1
				&& testNumberArray->elements[i][j] >= 1
				&& testNumberArray->elements[i][j] <= 9)
			{
				testNumberArray->isValid = 0;
				if(VERBOSE) printf("Array not valid! The duplicate is %d\n", testNumberArray->elements[i][j]);
				break;
			}
			// Element has not been seen
			else{
				if (testNumberArray->elements[i][j])
					numbersFound[testNumberArray->elements[i][j] - 1] = 1;
			}
		}
		// Break early since the structure is not valid
		if(testNumberArray->isValid == 0)
			break;
	}
	// Set isTested to 1 to let the dispatcher of the threads that this struct has been tested.
	testNumberArray->isTested = 1;
}

// REQUIRES: Valid 9x9 sudokuarray, 0 <= row <= 8
// MODIFIES: none
// EFFECTS: Returns a pointer to a dynamic copy of a column from sudokuarray
int *copyOfRow(int **sudokuarray, int row)
{
	int *temp = malloc(sizeof(int)*9);
	int i;

	for(i = 0; i < 9; i++)
	{
		temp[i] = sudokuarray[row][i];
		if(VERBOSE) printf("%d", temp[i]);
	}
	if(VERBOSE) printf("\n");
	return temp;
}

// REQUIRES: Valid 9x9 sudokuarray, 0 <= column <= 8
// MODIFIES: none
// EFFECTS: Returns a pointer to a dynamic copy of a column from sudokuarray
int *copyOfColumn(int **sudokuarray, int column)
{
	int *temp = malloc(sizeof(int)*9);
	int i;

	for(i = 0; i < 9; i++)
	{
		temp[i] = sudokuarray[i][column];
		if(VERBOSE) printf("%d\n", temp[i]);
	}
	//printf("\n");
	return temp;
}

// 0 1 2
// 3 4 5
// 6 7 8
// REQUIRES: Valid 9x9 sudokuarray, 0 <= blocknumber <= 8
// MODIFIES: none
// EFFECTS: Returns a pointer to a copy of a block sized portion of sudokuarray
// in heap memory
int *copyOfBlock(int **sudokuarray, int blocknumber)
{
	int *temp = malloc(sizeof(int)*9);
	// Correct vertical position to either 0, 3, or 6
	int vertical_pos = (blocknumber / 3) * 3;
	// Correct horizontal position to either 0, 3, or 6
	int horizontal_pos = blocknumber % 3 * 3;

	int i, j, k;

	// Iterate a 9x9 block leftwards/downwards to form the block
	i = 0;
	for(j = vertical_pos; j < vertical_pos + 3; j++)
	{
		for(k = horizontal_pos; k < horizontal_pos + 3; k++)
		{
			temp[i] = sudokuarray[j][k];
			if(VERBOSE) printf("%d", temp[i]);
			i++;
		}
		if(VERBOSE) printf("\n");
	}
	if(VERBOSE) printf("\n");
	return temp;
}



// REQUIRES: Non-initialized NumberArray structs
// MODIFIES: row_numberarrays, column_numberarrays, block_numberarrays
// EFFECTS: Initializes the structs for use, including pointers
void initializeArrays(NumberArray *row_numberarrays, NumberArray *column_numberarrays, NumberArray *block_numberarrays)
{
	int i;

	for(i = 0; i < NUM_ROW_THREADS; i++)
	{
		row_numberarrays[i].array_count = 0;
		row_numberarrays[i].isValid = 1;
		row_numberarrays[i].isTested = 0;
		// Allocate memory for the pointers of the pointers
		row_numberarrays[i].elements = malloc(sizeof(int *) * (SUDOKU_DEPTH / NUM_ROW_THREADS + 1));
	}

	for(i = 0; i < NUM_COL_THREADS; i++)
	{
		column_numberarrays[i].array_count = 0;
		column_numberarrays[i].isValid = 1;
		column_numberarrays[i].isTested = 0;
		// Allocate memory for the pointers of the pointers
		column_numberarrays[i].elements = malloc(sizeof(int *) * (SUDOKU_DEPTH / NUM_COL_THREADS + 1));
	}

	for(i = 0; i < NUM_BLOCK_THREADS; i++)
	{
		block_numberarrays[i].array_count = 0;
		block_numberarrays[i].isValid = 1;
		block_numberarrays[i].isTested = 0;
		// Allocate memory for the pointers of the pointers
		block_numberarrays[i].elements = malloc(sizeof(int *) * (SUDOKU_DEPTH / NUM_BLOCK_THREADS + 1));
	}
}


// REQUIRES: All the number array arguments passed to have initialized pointers
// MODIFIES: row_numberarrays, column_numberarrays, block_numberarrays
// EFFECTS: Free the dynamic memory within the structures
void freeNumberArrays(NumberArray *row_numberarrays, NumberArray *column_numberarrays, NumberArray *block_numberarrays)
{
	int i, j;
	for(i = 0; i < NUM_ROW_THREADS; i++)
	{
		for(j = 0; j < row_numberarrays[i].array_count; j++)
			free(row_numberarrays[i].elements[j]);

		free(row_numberarrays[i].elements);
	}
	for(i = 0; i < NUM_COL_THREADS; i++)
	{
		for(j = 0; j < column_numberarrays[i].array_count; j++)
			free(column_numberarrays[i].elements[j]);

		free(column_numberarrays[i].elements);
	}
	for(i = 0; i < NUM_BLOCK_THREADS; i++)
	{
		for(j = 0; j < block_numberarrays[i].array_count; j++)
			free(block_numberarrays[i].elements[j]);

		free(block_numberarrays[i].elements);
	}
}

// REQUIRES: sudokustring.length() == 81 && 0 <= sudokustring[i].toInt() <= 9
// MODIFIES: sudokuarray
// EFFECTS: Points sudokuarray into sudokustring to create a 9x9 array
int **decodeSudokuString(char *sudokustring)
{
	int i, j;

	int **sudokuarray;

	// Allocate memory for the two dimensional 9x9 array
	sudokuarray = malloc(sizeof(int*) * 9);
	for(i = 0; i < 9; i++)
		sudokuarray[i] = malloc(sizeof(int) * 9);

	for(i = 0; i < 9; i++){
		for(j = 0; j < 9; j++){
			// Correct ASCII to integer with 0x30
			sudokuarray[i][j] = sudokustring[i * 9 + j] - 0x30;
		}
	}
	return sudokuarray;
}

void printSudokuPuzzle(int ** sudokuarray)
{
	int i, j;

	for(i = 0; i < 9; i++){
		// Formatting to make it look nice
		if(i == 3 || i == 6)
			printf("---------------------\n");
		for(j = 0; j < 9; j++){
			printf("%c ", (!(sudokuarray[i][j]) ? '_' : (char) ((int) '0') + sudokuarray[i][j]));
			// More formatting
			if(j == 2 || j == 5)
				printf("| ");
		}
		printf("\n");
	}
	printf("\n");
}

// REQUIRES: Two valid integers
// MODIFIES: none
// EFFECTS: Returns the max of num1 and num2
int max(int num1, int num2)
{
	if(num1 >= num2)
		return num1;
	else
		return num2;
}

