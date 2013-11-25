#include "hostd.h"
#include <string.h>

void initSystem(void);
void closeSystem(void);
PCB newPCB(void);

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
        printf("fileBuffer: %s\n", fileBuffer);

        // Init a new process
        PCB process = newPCB();

        /*scan for process*/
        //add process info fill pcb with reqs
        printf("%d\n", process.io->printer);

        /*put process in dispatch queue*/
        //DEBUG
        /*replaceMe *process = malloc(sizeof(replaceMe));*/
        /**process = 8;*/
        /*enqueueProcess(dispatcher, process);*/
        /*free(process);*/
    }
    free (fileBuffer);
    fclose(fp);
}

// Initializes a new PCB for a process
// @par: none
// @ret: PCB    process pcb
PCB newPCB(void) {
    // Init a new PCB's status
    PCBStatus newPCBStatus;
    newPCBStatus.started = 1;
    newPCBStatus.running = 0;
    newPCBStatus.suspended = 0;
    newPCBStatus.terminated = 0;

    // Init a new PCB's io
    PCBIO newPCBIO;
    newPCBIO.printer = 0;
    newPCBIO.scanner = 0;
    newPCBIO.modem = 0;
    newPCBIO.disk = 0;

    // Init a new PCB
    PCB newPCB;
    newPCB.pid = -1;
    newPCB.priority = 0;
    newPCB.memory = 0;
    newPCB.arrivalTime = 0;
    newPCB.remainingTime = 0;
    newPCB.status = &newPCBStatus;
    newPCB.io = &newPCBIO;

    return newPCB;
}

// Initializes the system (global variables)
// @par: none
// @ret: none
void initSystem(void) {
    /*set resource counters*/
    /*set memory limit*/

    // Init all queues
    /*dispatcher = initializeQueue();*/
    dispatcher = realTimeQueue = userJobQueue = p1Queue = p2Queue = p3Queue = initializeQueue();

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
