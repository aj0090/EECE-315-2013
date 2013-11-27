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

   while(time < 100){
	printf("Time: %d\n", time);
	time++;
   }



    // DEBUG
    /*printQueue(dispatcher);*/

    closeSystem();

    return 0;
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

	// Static int to keep track of PID's
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
    newPCBIO->scannersHave = 0;
    newPCBIO->modemsHave = 0;
    newPCBIO->disksHave = 0;
    newPCBIO->memSpaceHave = 0;

    // Init a new PCB
    PCB *newPCB = malloc(sizeof(PCB));
    newPCB->pid = -1;
    newPCB->priority = 0;
    newPCB->arrivalTime = 0;
    newPCB->remainingTime = 0;
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
