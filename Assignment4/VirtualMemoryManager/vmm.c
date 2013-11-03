#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define PAGESIZE 256
#define PAGECOUNT 128
#define TLBSIZE 16
#define MAX_ADDRESSES 2000
#define MAX_CONCURRENT 3

int addressCursor = 0;
int pageFaults = 0, TLBHits = 0;
int numAddresses = 0;

pthread_mutex_t cursorLock;
pthread_mutex_t pageTableLock;

struct PageTable pageTable;
struct TLB tlb;
FILE *backingStore;

// pages and pageNumbers are associated with eachother by their index
struct PageTable{
	char *pages[PAGECOUNT];
	int pageNumbers[PAGECOUNT];
	int pageCount;
	int ageCounter[PAGECOUNT];
};


// pageNumbers and frameNumbers indexes are associated with eachother,
// i.e. index 0 will mean pageNumbers[0] corresponds with frameNumbers[0],
// and vice versa.
struct TLB{
	int pageNumbers[TLBSIZE];
	int frameNumbers[TLBSIZE];
	int tlbCount;
	int ageCounter[TLBSIZE];
};


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

int main(int argc, char **argv)
{

	if(argc < 2){
		printf("Insufficient arguments. Please enter just one argument, with the filename containing the addresses.\n");
		return 0;
	}

	int threaded = 0;


	// Check for multithreading
	if(argc == 3)
		if(!strcmp("--threaded", argv[2]))
			threaded = 1;

	// Open BACKING_STORE.bin into a file
	backingStore = NULL;
	backingStore = fopen("BACKING_STORE.bin", "r");

	if(backingStore == NULL){
		printf("BACKING_STORE.bin could not be opened!\n");
		return 0;
	}
	printf("BACKING_STORE.bin opened!\n");



	// Declare and initialize the PageTable
	ClearPageNumbers(&pageTable);

	// Declare and initialize the TLB
	ClearTLB(&tlb);


	// Prepare the list of addresses to be read
	int *addresses, addressCount;
	addresses = ReadAddresses(argv[1], &addressCount);

	numAddresses = addressCount;

	// Reading in the addresses failed, exit the program.
	if(addresses == NULL){
		printf("No addresses in list. Exiting program...\n");
		return 0;
	}

	if(!threaded){
		for(addressCursor = 0; addressCursor < numAddresses; addressCursor++)
		{
			// Read in the next address
			int address = addresses[addressCursor];

			// Find the pageNumber and Offset
			int pageNumber = GetPageNumber(address);
			int pageOffset = GetPageOffSet(address);

			char *page;

			// Check to see if the TLB contains a quick conversion from pageNumber to frameNumber
			int frameNumber = CheckTLB(&tlb, pageNumber);
	
			// frameNumber was not found in the TLB, so try looking if it's
			// in the pagetable
			if(frameNumber == -1){
				frameNumber = FindPageIndex(&pageTable, pageNumber);
			}
			else{
				TLBHits++;
			}

			// Framenumber found in the page tables.
			if(frameNumber != -1){
				page = pageTable.pages[frameNumber];
			}
			// frameNumber was not even in the pageTable, so read it from the backingStore
			else{
				page = ReadPage(backingStore, pageNumber);
				frameNumber= UpdatePageTable(&pageTable, page, pageNumber);
				pageFaults++;
			}


			// Print out the translations and the value
			printf("Virtual address: %d ", address);
			printf("Physical address: %d ", (frameNumber * 0x100 ) + pageOffset);
			printf("Value: %d\n", page[pageOffset]);


			// Update the TLB with the pageNumber to frameNumber translation
			UpdateTLB(&tlb, pageNumber, frameNumber);
		}

	}
	else{
		pthread_mutex_init(&cursorLock, NULL);
		pthread_mutex_init(&pageTableLock, NULL);

		pthread_t memoryThreads[MAX_CONCURRENT];

		int i;

		// Create row, column, and block threads
		for(i = 0; i < MAX_CONCURRENT; i++)
		{
			if(pthread_create(&memoryThreads[i], NULL, checkMemory, (&addresses) ))
				printf("Couldn't create thread for memoryThread %d.\n", i);

		}

		// Start execution on all of the threads
		for(i = 0; i < MAX_CONCURRENT; i++)
		{
			if(pthread_join(memoryThreads[i], NULL))
				printf("Couldn't create thread for memoryThread %d.\n", i);
		}
	}


	printf("Number of Translated Addresses = %d\n", numAddresses);

	printf("Page Faults = %d\n", pageFaults);
	printf("Page Fault Rate = %0.3f\n", (float)pageFaults / (float)numAddresses);

	printf("TLB Hits = %d\n", TLBHits);
	printf("TLB Hit Rate = %0.3f\n", (float)TLBHits / (float)numAddresses);
	
	free(addresses);

	FreePageTable(&pageTable);

	fclose(backingStore);
}


// REQUIRES: Valid pointer to a TLB struct
// MODIFIES: tlb
// EFFECTS: Sets tlb->frameNumbers all to -1, sets tlb->pageNumbers all to -1,
// and sets tlb->tlbCount to 0.
void ClearTLB(struct TLB *tlb)	
{
	tlb->tlbCount = 0;
	int i;
	for (i = 0; i < TLBSIZE; i++) {
		tlb->frameNumbers[i] = -1;
		tlb->pageNumbers[i] = -1;
		tlb->ageCounter[i] = 0;
	}
	return;
}

// REQUIRES: pageNumber is between 0 and PAGECOUNT
// MODIFIES: none
// EFFECTS: Checks to see if the TLB contains a translation for pageNumber,
// If it does, return the translation. If not, return -1
int CheckTLB(struct TLB *tlb, int pageNumber)
{
	int i;
	for (i = 0; i < tlb->tlbCount; i++) {
		if (tlb->pageNumbers[i] == pageNumber)
			return tlb->frameNumbers[i];
	}
	//TODO: IMPLEMENT ME
	return -1;
}


// REQUIRES: pageNumber and frameNumber are both between 0 and PAGECOUNT
// MODIFIES: tlb
// EFFECTS: Updates the TLB, with pageNumber and frameNumber. The eviction
// policy choice is yours (FIFO, or w/e you want :) ).
// Feel free to modify the TLB struct in order to accomplish what you want.
void UpdateTLB(struct TLB *tlb, int pageNumber, int frameNumber)
{
	int i;
	int max = 0;
	int oldestIndex = 0;
	if (tlb->tlbCount == TLBSIZE)
	{
		for (i = 0; i < TLBSIZE; i++) {
			if (tlb->ageCounter[i] >= max) {
				max = tlb->ageCounter[i];
				oldestIndex = i;
			}
		}
		tlb->pageNumbers[oldestIndex] = pageNumber;
		tlb->frameNumbers[oldestIndex] = frameNumber;
		tlb->ageCounter[oldestIndex] = 0;
	} else {
		tlb->pageNumbers[tlb->tlbCount] = pageNumber;
		tlb->frameNumbers[tlb->tlbCount] = frameNumber;
		tlb->tlbCount++;
	}

	for (i = 0; i < tlb->tlbCount; i++) {
		tlb->ageCounter[i]++;
	}	

	return;
}


// REQUIRES: Valid PageTable
// MODIFIES: pageTable
// EFFECTS: Sets all array elements of pageTable->pageNumbers to -1, and sets
// pageTable->pageCount to 0
void ClearPageNumbers(struct PageTable *pageTable)
{
	pageTable->pageCount = 0;
	int i;
	for (i = 0; i < PAGECOUNT; i++) {
		pageTable->pageNumbers[i] = -1;
		pageTable->ageCounter[i] = 0;
	}	

	return;
}


// REQUIRES: Valid pageTable, pageNumber is between 0 and 255
// MODIFIES: None
// EFFECTS: Returns the index of pageNumber->pageNumbers that 
// contains pageNumber. Else, returns -1.
int FindPageIndex(struct PageTable *pageTable, int pageNumber)
{
	int i;
	for (i = 0; i < pageTable->pageCount; i++) {
		if (pageTable->pageNumbers[i] == pageNumber)
			return i;
	}	
	//TODO: IMPLEMENT ME
	return -1;
}


// REQUIRES: Initialized and allocated page from BACKING_STORE.bin, and 
// pageNumber is between 0 and PAGECOUNT
// MODIFIES: pageTable
// EFFECTS: STORES 
int AddPage(struct PageTable *pageTable, char *page, int pageNumber)
{
	pageTable->pages[pageTable->pageCount] = page;
	pageTable->pageNumbers[pageTable->pageCount] = pageNumber;
	pageTable->pageCount++;
	return pageTable->pageCount - 1;
}

// REQUIRES: pageNumber is between 0 and PAGECOUNT
// MODIFIES: pageTable
// EFFECTS: Updates the pageTable with new page and evicts the oldest one when needed
int UpdatePageTable(struct PageTable *pageTable, char *page, int pageNumber)
{
	int i;
	int max = 0;
	int oldestIndex = 0;
	int frameNumber;
	if (pageTable->pageCount == PAGECOUNT)
	{
		for (i = 0; i < PAGECOUNT; i++) {
			if (pageTable->ageCounter[i] >= max) {
				max = pageTable->ageCounter[i];
				oldestIndex = i;
			}
		}
		free(pageTable->pages[oldestIndex]);
		pageTable->pages[oldestIndex] = page;
		pageTable->pageNumbers[oldestIndex] = pageNumber;
		pageTable->ageCounter[oldestIndex] = 0;
		frameNumber = oldestIndex;
	} else {
		pageTable->pages[pageTable->pageCount] = page;
		pageTable->pageNumbers[pageTable->pageCount] = pageNumber;
		frameNumber = pageTable->pageCount;
		pageTable->pageCount++;
	}

	for (i = 0; i < pageTable->pageCount; i++) {
		pageTable->ageCounter[i]++;
	}	

	return frameNumber;
}


// REQUIRES: none
// MODIFIES: pageTable
// EFFECTS: Frees the memory allocated to each specific page stored
// in the pageTable.
void FreePageTable(struct PageTable *pageTable)
{
	int i;
	for (i = 0; i < pageTable->pageCount; ++i)
		free(pageTable->pages[i]);

	return;
}


// REQUIRES: none
// MODIFIES: none
// EFFECTS: Shifts bits 8-15 of address to the right so that bit 8
// is now bit 0, and then masks the 8 bits
int GetPageNumber(int address)
{
	// Return the bits 8-15, shifted to the right
	return (address / 0x100) & 0xFF;
}

// REQUIRES: none
// MODIFIES: none
// EFFECTS: Masks the address so that only bits 0-7 are returned
int GetPageOffSet(int address)
{
	// Return just the bits 0-7
	return address & 0XFF;
}


// REQUIRES: addressCount is a valid pointer back to memory
// allocated in the calling function
// MODIFIES: addressCount
// EFFECTS: Reads in a sequence of addresses ranging from int values
// 0-65535, in string form, seperated by new lines. On finish, returns
// a pointer to an allocated int array with all of the values, and 
// sets addressCount to the number of addresses in the array.
int* ReadAddresses(char *fileName, int *addressCount)
{
	int *addresses = (int*)malloc(sizeof(int)*MAX_ADDRESSES);

    FILE * f = fopen(fileName, "r");

    int i;
    for(i = 0; i < MAX_ADDRESSES; i++)
    	if (fscanf(f, "%d", &addresses[i]) != 1) {
    		*addressCount = i;
    		return addresses;
    	}

	return 0;
}



// REQUIRES: backingStore is an opened file set to "BACKING_STORE.bin"
// MODIFIES: none
// EFFECTS: Sets the cursor of backingStore to the offset, and then reads
// the next PAGESIZE bytes into a buffer, and returns the buffer. 
// If fread() fails, return NULL
char *ReadPage(FILE *backingStore, int offset)
{
	if(offset < 0 || offset >= PAGESIZE)
	{
		printf("Address not in range (inside ReadPage)\n");
		return NULL;
	}

	char *buffer = malloc(PAGESIZE);

	fseek(backingStore, offset*PAGESIZE, SEEK_SET);

	if(!fread(buffer, sizeof(char), PAGESIZE, backingStore))
	{
		return NULL;
	}

	return buffer;

}


void *checkMemory(void *arg)
{
	int **addresses = arg;

	while (addressCursor < numAddresses) {
		// Read in the next address
		pthread_mutex_lock(&cursorLock);
		int address = (*addresses)[addressCursor++];
		pthread_mutex_unlock(&cursorLock);

		// Find the pageNumber and Offset
		int pageNumber = GetPageNumber(address);
		int pageOffset = GetPageOffSet(address);

		char *page;

		// Check to see if the TLB contains a quick conversion from pageNumber to frameNumber
		pthread_mutex_lock(&pageTableLock);
		int frameNumber = CheckTLB(&tlb, pageNumber);

		// frameNumber was not found in the TLB, so try looking if it's
		// in the pagetable
		if(frameNumber == -1){
			frameNumber = FindPageIndex(&pageTable, pageNumber);
		}
		else{
			TLBHits++;
		}

		// Framenumber found in the page tables.
		if(frameNumber != -1){
			page = pageTable.pages[frameNumber];
		}
		// frameNumber was not even in the pageTable, so read it from the backingStore
		else{
			page = ReadPage(backingStore, pageNumber);
			frameNumber= UpdatePageTable(&pageTable, page, pageNumber);
			pageFaults++;
		}


		// Print out the translations and the value
		printf("Virtual address: %d ", address);
		printf("Physical address: %d ", (frameNumber * 0x100 ) + pageOffset);
		printf("Value: %d\n", page[pageOffset]);


		// Update the TLB with the pageNumber to frameNumber translation
		UpdateTLB(&tlb, pageNumber, frameNumber);
		pthread_mutex_unlock(&pageTableLock);
	}


}
