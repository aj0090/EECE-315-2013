#include "hostd.h"
#include <string.h>

void initSystem(void);
void closeSystem(void);
PCB *newPCB(void);
void freePCB(PCB *);

int main(int argc, char **argv) {
    if (argc != 2) {
        // Print directions if the file is not supplied and exit
        printf("You must supply a file to the program:\n"
                "$ hostd filename.txt\n");
        exit(0);
    }

    initSystem();

    char *filename = argv[1];
    readFile(filename, dispatcher);
    printf("done...\n");


   int time = 0;
   int rtUpdated;

   while(time < 25){
	printf("Time: %d\n", time);
	rtUpdated = updateDispatcher(time);

	doProcessing(time, rtUpdated);
	
	time++;
	//sleep(1);
   }



    // DEBUG
    /*printQueue(dispatcher);*/

	printf("Current Memory map: \n");
	int i;
	for(i = 0; i < host.memSpaceMax; i++)
	{
		printf("%d ", host.memSpace[i]);
	}
	printf("\n");

    printf("REAL TIME QUEUE: ");
    printQueue(realTimeQueue);
    printf("PRIORITY 1 QUEUE: ");
    printQueue(p1Queue);

    printf("PRIORITY 2 QUEUE: ");
    printQueue(p2Queue);

    printf("PRIORITY 3 QUEUE: ");
    printQueue(p3Queue);

    closeSystem();

    return 0;
}




void doProcessing(int time, int rtUpdated)
{
	// Switching processes goes here
/*
	printf("HELLO!\n");

	if(host.currentProcess == NULL && time >= 12){
		printf("current process is null.\n");
		host.currentProcess = p1Queue->process;
		dequeueProcess(&p1Queue);
	}
	if(host.currentProcess != NULL){
		printf("host printers: %d\n", host.numPrinters);
		printf("current printer: %d\n", host.currentProcess->IO->printerStartID);
		freeHostResources(host.currentProcess);
	}
*/
	// REPLACEMENT CHECK

	printf("IN PROCESSOR\n");
		

	printf("rtUpdated: %d\n", rtUpdated);
	if(host.currentProcess != NULL){
		
		// Current process priority is not zero, therefore not realtime
		if(host.currentProcess->priority != 0){

			// If realtime processes were updated
			if(rtUpdated)
			{
				// Move the process back to its queue, and keep the remaining timesplice on it
				host.currentProcess->state = SUSPENDED;
				enqueueToPriority(host.currentProcess);

				// Enqueue the realtime process
				if(realTimeQueue->process != NULL)
				{
					host.currentProcess = realTimeQueue->process;
					host.currentProcess->timeSpliceLeft = host.currentProcess->remainingTime;
					dequeueProcess(&realTimeQueue);
				}
			}
			
		}

	}
	// Processor does not have a process currently
	else{
		printf("NO PROCESS, LETS FIND ONE\n");
		if(rtUpdated){
			// Enqueue the realtime process
			if(realTimeQueue){
				if(realTimeQueue->process != NULL)
				{
					host.currentProcess = realTimeQueue->process;
					host.currentProcess->timeSpliceLeft = host.currentProcess->remainingTime;
					dequeueProcess(&realTimeQueue);
		
				}
			}
		}
		else if (p1Queue) {
			if(p1Queue->process != NULL)
			{
				host.currentProcess = p1Queue->process;
				dequeueProcess(&p1Queue);
				host.currentProcess->timeSpliceLeft = P1_TIMEQUANTUM;
			}
		}
		else if(p2Queue) { 
			if(p2Queue->process != NULL)
			{
				host.currentProcess = p2Queue->process;
				dequeueProcess(&p2Queue);
				host.currentProcess->timeSpliceLeft = P2_TIMEQUANTUM;
			}
		}
		else if(p3Queue) {
			if(p3Queue->process != NULL)
			{
				host.currentProcess = p3Queue->process;
				dequeueProcess(&p3Queue);
				host.currentProcess->timeSpliceLeft = P3_TIMEQUANTUM;
			}
		}
		else{
			return;		
		}
	
	}

	// Actual processing goes here

	if(host.currentProcess != NULL){
		host.currentProcess->state = RUNNING;
		host.currentProcess->timeSpliceLeft--;
		host.currentProcess->remainingTime--;
		printf("PID: %d\n", host.currentProcess->pid);
		printf("Time splice left: %d, remaining time: %d\n", host.currentProcess->timeSpliceLeft, host.currentProcess->remainingTime);
		
		// Process has no time splice left
		if(host.currentProcess->remainingTime == 0)
		{
			printf("Process finished running!\n");
			freeHostResources(host.currentProcess);
			host.currentProcess->state = TERMINATED;
			free(host.currentProcess);
			host.currentProcess = NULL;
		}
		else if(host.currentProcess->timeSpliceLeft == 0)
		{
			printf("WHAT\n");
			if(host.currentProcess->priority < 3 && host.currentProcess->priority >= 1)
			{
				printf("huh.\n");
				host.currentProcess->priority++;
				enqueueToPriority(host.currentProcess);
				host.currentProcess = NULL;
			}
			else if(host.currentProcess->priority >= 3)
			{	
				host.currentProcess->timeSpliceLeft = P3_TIMEQUANTUM;	
				host.currentProcess->priority = 1;
				enqueueToPriority(host.currentProcess);
				host.currentProcess = NULL;
			}
		}
	}


	// Process termination/transcending of queues goes here
}



// Returns 1 if theres a new process placed in the Realtime Queue,
// else returns 0
int updateDispatcher(int time)
{
    // Create a pointer to the dispatcher so we don't modify dispatcher directly 
    // unless we need to
    Queue *queuePtr = dispatcher;

    // Create a new queue for elements that weren't able to be added.
    Queue *newDispatcher = initializeQueue();
    int rtQUpdated = 0;

    if (!(queuePtr)) {
        printf("The queue is empty!\n\n");
        return 0;
    }

    // Iterate through each element in the queue
    int i = 0;
    int count = numElems(queuePtr);

    // If the first process is NULL, we don't want to iterate through them
    // (double check that there are elements in queue)
    if(queuePtr->process == NULL)
	i = count;

    // Keep an indicator to see if anything breaks
    int broken = 0;
    //printf("Num elems: %d\n", count);

    // Iterate through each element in the dispatcher
    while (i < count) {

	
	//printf("%d/%d\n", i + 1, count);
	//dequeueProcess(&queuePtr);
		
        // Dequeue the next process from the queue
	PCB *currentProcess = dequeueProcess(&queuePtr)->process;

        // Queue is empty.
        // (Triple check)
	if(currentProcess == NULL){
		printf("BROKEN!\n");
		broken = 1;
		break;
	}

	// If the process has "arrived", attempt to allocate resources to it
	if(time >= currentProcess->arrivalTime){
		//printf("HELLO from PID: %d!\n", currentProcess->pid);
		// Attempt to allocate resources to the process.
		int allocResult = allocateResources(currentProcess); 

		// Resources were allocated successfully
		if(allocResult)
		{
			// Send the process to join the appropriate priority queue
			printf("Successful add!\n");
			enqueueToPriority(currentProcess);
			if(currentProcess->priority == 0)
				rtQUpdated = 1;
			printf("RT: %d\n", rtQUpdated);
		}
 		// Resources could not be allocated for it.
		else
		{
			// Throw the process back on the dispatcher queue
			printf("Not enough resources!\n");
			enqueueProcess(newDispatcher, currentProcess);
		}
	}
	// The time has not come for this process, add it to the new dispatcher.
	else{
		enqueueProcess(newDispatcher, currentProcess);
	}
	i++;
    }
    // If the queue wasn't broken, 
    if(!broken){
	    if(dispatcher == NULL){
	    	cleanQueue(dispatcher);
	    }
	    dispatcher = newDispatcher;
    }
    //printf("here?\n");
    return rtQUpdated;
}


void enqueueToPriority(PCB *process)
{
	int p = process->priority;
	if(p == 0)
	{
		if(!realTimeQueue)
			realTimeQueue = initializeQueue();
		enqueueProcess(realTimeQueue, process);
	}
	else if(p == 1)
	{
		if(!p1Queue)
			p1Queue = initializeQueue();
		enqueueProcess(p1Queue, process);
	}
	else if(p == 2)
	{
		if(!p2Queue)
			p2Queue = initializeQueue();
		enqueueProcess(p2Queue, process);
	}
	else
	{
		if(!p3Queue)
			p3Queue = initializeQueue();
		enqueueProcess(p3Queue, process);
	}

	return;
}


// Returns 2 if hostd contains enough resources and process is realtime
// Returns 1 if hostd contains enough resources
// Returns 0 if not
int allocateResources(PCB *process)
{
    int a, b, c, d, e;
    
    //printf("Printer Allocated: %d\n", allocatePrinters(process));
    a = allocatePrinters(process);
    b = allocateScanners(process);
    c = allocateModems(process);
    d = allocateDisks(process);

    int realTime = (process->priority == 0);

    if(realTime){
	e = allocateRealTimeMemory(process);
    }
    else{
	e = allocateMemory(process);
    }


    if(a&&b&&c&&d&&e){
	
	return realTime ? 2 : 1;
    }
    else{
	freeHostResources(process);
	return 0;
    }

}


// REQUIRES: none
// MODIFIES: process
// EFFECTS: Frees the processes resources from the host.
void freeHostResources(PCB *process)
{
	int i = 0;
	printf("Freeing processes resources!\n");

	printf("host printers: %d\n", host.numPrinters);

	printf("Printers: %d/%d", process->IO->printerStartID, host.numPrinters);
	
	if(process->IO->printerStartID != -1){
		for(i = process->IO->printerStartID; i < process->IO->printerStartID + process->IO->printersHave ; i++){
			host.printersAlloc[i] = 0;
		}
		printf("Printer freed!\n");
	}
	if(process->IO->scannerStartID != -1){
		for(i = process->IO->scannerStartID; i < (process->IO->scannerStartID + process->IO->scannersHave); i++){
			host.scannersAlloc[i] = 0;
		}
		printf("Scanner freed!\n");
	}

	if(process->IO->modemStartID != -1){
		for(i = process->IO->modemStartID; i < (process->IO->modemStartID + process->IO->modemsHave); i++){
			host.modemsAlloc[i] = 0;
		}
		printf("modem freed!\n");
	}

	if(process->IO->diskStartID != -1){
		for(i = process->IO->diskStartID; i < (process->IO->diskStartID + process->IO->disksHave); i++){
			host.disksAlloc[i] = 0;
		}
		printf("CD drive freed!\n");
	}

	if(process->IO->memSpaceStartID != -1){
		for(i = process->IO->memSpaceStartID; i < (process->IO->memSpaceStartID + process->IO->memSpaceHave); i++){
			host.memSpace[i] = 0;
		} 
	}


	printf("Finished freeing resources!\n");
}


// Returns an index where Resources can be allocated
// Returns -1 if no printers are available
int checkResourcesFreeFirstFit(int numResourcesReq, int pid, int numHostResources, int *hostResources)
{
	int i;
	int cursor = -1, count = 0;
	for(i = 0; i < numHostResources; i++)
	{	
		// 0 signifies no process is using it
		if(hostResources[i] == 0)
		{
			// If the last element wasn't free, set the printerCursor 
			// to the current element
			if(cursor == -1)
			{
				cursor = i;
			}
			
			count++;
			// Check if the desired resource count was met
			if(count == numResourcesReq){
				break;
			}
		}
		// This shouldn't happen in the current version, but 
		else if(hostResources[i] == pid){
			return -1;
		}
		// Allocated memory block found. Set the current count to zero and make the
		// printerCursor = -1.
		else{
			cursor = -1;
			count = 0;
		}
	}
	// Enough resources were found
	if(count == numResourcesReq)
		return cursor;
	// Not enough resources were found
	else{
		return -1;
	}
}



// Returns 1 if printers were allocated correctly
// Returns 0 otherwise
int allocatePrinters(PCB *process)
{
	// No printers are needed, therefore return true
	if(process->IO->printersNeeded == 0)
		return 1;


	int startCursor = checkResourcesFreeFirstFit(process->IO->printersNeeded, process->pid, host.numPrinters, host.printersAlloc);
	int i;


	// Adequate resources are available starting at startCursor
	if(startCursor != -1)
	{
		// Allocate the printers with the process PID
		for(i = startCursor; i < startCursor + process->IO->printersNeeded; i++){
			host.printersAlloc[i] = process->pid;
		}
		// Update the number of printers inside the process
		process->IO->printersHave = process->IO->printersNeeded;
		printf("Allocated %d printers starting at printer #%d!\n", process->IO->printersHave, startCursor);
		process->IO->printerStartID = startCursor;
		return 1;
	}
	
	return -1;
}

// Returns 1 if scanners were allocated correctly
// Returns 0 otherwise
int allocateScanners(PCB *process)
{
	// No scanners are needed, therefore return true
	if(process->IO->scannersNeeded == 0)
		return 1;


	int startCursor = checkResourcesFreeFirstFit(process->IO->scannersNeeded, process->pid, host.numScanners, host.scannersAlloc);
	int i;


	// Adequate resources are available starting at startCurso


	if(startCursor != -1)
	{
		// Allocate the scannerss with the process PID
		for(i = startCursor; i < startCursor + process->IO->scannersNeeded; i++){
			host.scannersAlloc[i] = process->pid;
		}
		// Update the number of scanners inside the process
		process->IO->scannersHave = process->IO->scannersNeeded;
		printf("Allocated %d scanners starting at scanner #%d!\n", process->IO->scannersHave, startCursor);
		process->IO->scannerStartID = startCursor;
		return 1;
	}
	
	return -1;
}


// Returns 1 if modems were allocated correctly
// Returns 0 otherwise
int allocateModems(PCB *process)
{
	// No modems are needed, therefore return true
	if(process->IO->modemsNeeded == 0)
		return 1;


	int startCursor = checkResourcesFreeFirstFit(process->IO->modemsNeeded, process->pid, host.numModems, host.modemsAlloc);
	int i;


	// Adequate resources are available starting at startCursor
	if(startCursor != -1)
	{
		// Allocate the scannerss with the process PID
		for(i = startCursor; i < startCursor + process->IO->modemsNeeded; i++){
			host.modemsAlloc[i] = process->pid;
		}
		// Update the number of scanners inside the process
		process->IO->modemsHave = process->IO->modemsNeeded;
		printf("Allocated %d modems starting at modem #%d!\n", process->IO->modemsHave, startCursor);
		process->IO->modemStartID = startCursor;
		return 1;
	}
	
	return -1;
}


// Returns 1 if diskss were allocated correctly
// Returns 0 otherwise
int allocateDisks(PCB *process)
{
	// No modems are needed, therefore return true
	if(process->IO->disksNeeded == 0){
		//printf("No disks needed.\n");
		return 1;
	}


	int startCursor = checkResourcesFreeFirstFit(process->IO->disksNeeded, process->pid, host.numDisks, host.disksAlloc);
	int i;


	// Adequate resources are available starting at startCursor
	if(startCursor != -1)
	{
		// Allocate the scannerss with the process PID
		for(i = startCursor; i < startCursor + process->IO->disksNeeded; i++){
			host.disksAlloc[i] = process->pid;
		}
		// Update the number of scanners inside the process
		process->IO->disksHave = process->IO->disksNeeded;
		printf("Allocated %d disks starting at disk #%d!\n", process->IO->disksHave, startCursor);
		process->IO->diskStartID = startCursor;
		return 1;
	}
	
	return -1;
}


int allocateRealTimeMemory(PCB *process)
{
	// No modems are needed, therefore return true
	if(process->IO->memSpaceNeeded == 0){
		printf("No memory needed.... weird\n");
		return 1;
	}


	//printf("hi\n");
	// Pass the same array, but only let it see the first 96 bytes (since this is
	// is a real time process)
	int startCursor = checkResourcesFreeFirstFit(process->IO->memSpaceNeeded, process->pid, host.rtMemSpaceMax, host.memSpace);
	int i;


	//printf("startcursor: %d, %d/%d\n", startCursor, process->IO->memSpaceNeeded, host.rtMemSpaceMax);
	// Adequate resources are available starting at startCursor
	if(startCursor != -1)
	{
		//printf("PID: %d\n", process->pid);
		//memset(host.memSpace + startCursor, process->pid, process->IO->memSpaceNeeded);
		// Allocate the scannerss with the process PID
		for(i = startCursor; i < startCursor + process->IO-> memSpaceNeeded; i++){
			host.memSpace[i] = process->pid;
		}
		// Update the number of scanners inside the process
		process->IO->memSpaceHave = process->IO->memSpaceNeeded;
		printf("Allocated %d MByte's starting at address #%d!\n", process->IO->memSpaceHave, startCursor);
		process->IO->memSpaceStartID = startCursor;
		return 1;
	}
	
	return -1;
}


int allocateMemory(PCB *process)
{
	// No modems are needed, therefore return true
	if(process->IO->memSpaceNeeded == 0){
		printf("No memory needed.... weird\n");
		return 1;
	}


	// Pass the same array, but pass the start as 96 bytes forward(ignore RT memory)
	// Also, show that the length is 96 bytes less(rt memory)
	int startCursor = checkResourcesFreeFirstFit(process->IO->memSpaceNeeded, process->pid, host.memSpaceMax - host.rtMemSpaceMax, host.memSpace + host.rtMemSpaceMax);
	int i;

	// Adequate resources are available starting at startCursor
	if(startCursor != -1)
	{
		// Allocate the scannerss with the process PID
		for(i = startCursor + host.rtMemSpaceMax; i < startCursor + process->IO->memSpaceNeeded + host.rtMemSpaceMax; i++){
			host.memSpace[i] = process->pid;
		}
		// Update the number of scanners inside the process
		process->IO->memSpaceHave = process->IO->memSpaceNeeded;
		printf("Allocated %d MByte's starting at address #%d!\n", process->IO->memSpaceHave, startCursor + host.rtMemSpaceMax);
		process->IO->memSpaceStartID = startCursor + host.rtMemSpaceMax;
		return 1;
	}
	
	return -1;
	
}



// Creates processes from the file given and adds them to the dispatcher queue
// @par: pointer        filename
// @par: Queue pointer  dispatcher queue
// @ret: none
void readFile(char *filename, Queue *dispatcher) {
    FILE *fp;
    fp = fopen(filename, "r");
    if (!fp) {
        // Print error if the file was not found and exit
        printf("The file %s was not found, exiting.\n", filename);
        exit(0);
    } else {
        printf("The file %s was found.\n", filename);
    }

    char *fileBuffer = malloc(FILE_BUFFER);

    while (fgets(fileBuffer, FILE_BUFFER, fp) != NULL) {
        fileBuffer = strtok(fileBuffer, "\n");
        //printf("fileBuffer: %s\n", fileBuffer);

        // Init a new process
        PCB *process = newPCB();

        /*scan for process*/
        //add process info fill pcb with reqs

	int *args = stringToInts(fileBuffer);
	// 0 - arrival time
        // 1 - priority
	// 2 - processor time
        // 3 - Mbytes
        // 4 - # printers
        // 5 - # scanners
        // 6 - # modems
	// 7 - # cds

	// Static int to keep track of PID's, starting at id = 1
	static int pid = 1;    

	// Allocate the pid and the args to the struct members
	process->pid = pid++;
    	process->priority = args[1];
    	process->arrivalTime = args[0];
    	process->remainingTime = args[2];
    	process->state = STARTED;


	// Allocate the requires resources for the process.
   	process->IO->memSpaceNeeded = args[3];
    	process->IO->printersNeeded = args[4];
    	process->IO->scannersNeeded = args[5];
    	process->IO->modemsNeeded = args[6];
	process->IO->disksNeeded = args[7];

	printPCBInfo(process);

	free(args);

        /*put process in dispatch queue*/
        //DEBUG
        /*replaceMe *process = malloc(sizeof(replaceMe));*/
        /**process = 8;*/
        enqueueProcess(dispatcher, process);
        /*free(process);*/
    }
    printQueue(dispatcher);
    free (fileBuffer);
    fclose(fp);
}


int *stringToInts(char *string)
{ 
    // Allocate new array with size 8 since file input includes 8 ints
    int *numbers = malloc(sizeof(int) * 8);
    char *pt;
    pt = strtok (string,", ");

    int count = 0;
    while (pt != NULL && count < 8) {
        numbers[count++] = atoi(pt);
        pt = strtok (NULL, ", ");
    }
    return numbers;
}


void printPCBInfo(PCB *process)
{
    printf("PID: %d\n", process->pid);
    printf("\tPRIORITY: \t%d\n", process->priority);
    printf("\tARRIVAL TIME: \t%d\n", process->arrivalTime);
    printf("\tREMAINING TIME: %d\n", process->remainingTime);
    printf("\tSTATE: \t\t%d\n", process->state);

    printf("IO:\tPRINTERS: \t%d / %d\n", process->IO->printersHave, process->IO->printersNeeded);
    printf("\tSCANNERS: \t%d / %d\n", process->IO->scannersHave, process->IO->scannersNeeded);
    printf("\tMODEMS: \t%d / %d\n", process->IO->modemsHave, process->IO->modemsNeeded);
    printf("\tDISKS: \t\t%d / %d\n", process->IO->disksHave, process->IO->disksNeeded);
    printf("\tMEMORY: \t%d / %d\n", process->IO->memSpaceHave, process->IO->memSpaceNeeded);
    
}


// Initializes a new PCB for a process
// @par: none
// @ret: PCB    process pcb
PCB *newPCB(void) {

    // Init a new PCB's io
    PCBIO *newPCBIO = malloc(sizeof(PCBIO));

    newPCBIO->printersHave = 0;
    newPCBIO->printerStartID = -1;

    newPCBIO->scannersHave = 0;
    newPCBIO->scannerStartID = -1;

    newPCBIO->modemsHave = 0;
    newPCBIO->modemStartID = -1;

    newPCBIO->disksHave = 0;
    newPCBIO->diskStartID = -1;

    newPCBIO->memSpaceHave = 0;
    newPCBIO->memSpaceStartID = -1;

    // Init a new PCB
    PCB *newPCB = malloc(sizeof(PCB));
    newPCB->pid = -1;
    newPCB->priority = 0;
    newPCB->arrivalTime = 0;
    newPCB->remainingTime = 0;

    newPCB->timeSpliceLeft = 0; 

    newPCB->state = STARTED;
    newPCB->IO = newPCBIO;

    return newPCB;
}

void freePCB(PCB *process) {

    // TODO: Free hosts memory here




    // Free the PCB's actual memory
    free(process->IO);
    free(process);

}


// Initializes the system (global variables)
// @par: none
// @ret: none
void initSystem(void) {
    /*set resource counters*/
    /*set memory limit*/

    int i;
    host.numPrinters = MAX_PRINTERS;
    host.numScanners = MAX_SCANNERS;
    host.numModems = MAX_MODEMS;
    host.numDisks = MAX_DISKS;
    host.memSpaceMax = MAX_MEMSPACE;
    host.rtMemSpaceMax = MAX_RTMEMSPACE;
    host.rtMemSpaceUsed = 0;
    host.currentProcess = NULL;

    memset(host.printersAlloc, 0, MAX_PRINTERS);
    memset(host.scannersAlloc, 0, MAX_SCANNERS);
    memset(host.modemsAlloc, 0, MAX_MODEMS);
    memset(host.disksAlloc, 0, MAX_DISKS);
    memset(host.memSpace, 0, MAX_MEMSPACE);




    // Init all queues to be separate queues 
    dispatcher = initializeQueue();
    realTimeQueue = initializeQueue();
    userJobQueue = initializeQueue();
    p1Queue = initializeQueue();
    p2Queue = initializeQueue();
    p3Queue = initializeQueue();

    // Set CPU start time to 0
    cpuTime = 0;
}

// Shuts down the system (global variables) by freeing memory
// @par: none
// @ret: none
void closeSystem(void) {
    /*reset resource counters?*/
    /*reset memory limit?*/

    // Empty all queues
    cleanQueue(dispatcher);
}
