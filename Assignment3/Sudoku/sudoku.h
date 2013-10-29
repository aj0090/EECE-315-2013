#ifndef SUDOKU_H
#define SUDOKU_H

// Includes
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <math.h>
#include <time.h>

// Constants

#define NUM_COL_THREADS 3
#define NUM_ROW_THREADS 3
#define NUM_BLOCK_THREADS 3

#define BEGINNER 64
#define EASY 48
#define MEDIUM 36
#define HARD 24
#define VERY_HARD 17

#define SUDOKU_DEPTH 9

// Define VERBOSE to be 1 if you want all the debugging outputs
#define VERBOSE 0

// Struct definitions

typedef struct NumberArray{
    int **elements;
    int array_count;
    int isValid;
    int isTested;
} NumberArray;

// Function prototypes

int testSudokuString(char *);
void generateSudokuPuzzle(int);
int **rotateArray(int**);
void swapRows(int***);

int testNumberArrays(NumberArray *, NumberArray *, NumberArray *);
void *isValid(void *);
int *copyOfRow(int **, int);
int *copyOfColumn(int **, int);
int *copyOfBlock(int **, int);

void initializeArrays(NumberArray *, NumberArray *, NumberArray *);
void freeNumberArrays(NumberArray *, NumberArray *, NumberArray *);

int **decodeSudokuString(char *);
void printSudokuPuzzle(int **);

int max(int, int);

#endif
