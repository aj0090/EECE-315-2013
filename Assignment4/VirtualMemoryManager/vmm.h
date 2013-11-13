#ifndef VMM_H
#define VMM_H

// Includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

// Constants
#define PAGESIZE 256
#define PAGECOUNT 128
#define TLBSIZE 16
#define MAX_ADDRESSES 2000
#define MAX_CONCURRENT 3

// Struct definitions
// Pages and pageNumbers are associated with eachother by their index
struct PageTable{
    char *pages[PAGECOUNT];
    int pageNumbers[PAGECOUNT];
    int pageCount;
    int ageCounter[PAGECOUNT];
};

// PageNumbers and frameNumbers indexes are associated with eachother,
// i.e. index 0 means pageNumbers[0] corresponds with frameNumbers[0],
// and vice versa.
struct TLB{
    int pageNumbers[TLBSIZE];
    int frameNumbers[TLBSIZE];
    int tlbCount;
    int ageCounter[TLBSIZE];
};

// Function Prototypes
void ClearTLB(struct TLB *tlb);
int CheckTLB(struct TLB *, int);
void UpdateTLB(struct TLB *, int, int);

void ClearPageNumbers(struct PageTable *);
int FindPageIndex(struct PageTable *, int p);
int AddPage(struct PageTable *, char *, int);
int UpdatePageTable(struct PageTable *pageTable, char *page, int pageNumber);
void FreePageTable(struct PageTable *);

int GetPageNumber(int);
int GetPageOffSet(int);

int *ReadAddresses(char *fileName, int *addressCount);
char *ReadPage(FILE *, int);

void *checkMemory(void *arg);

#endif
