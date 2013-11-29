#include "../hostd.h"

// Initializes a new PCB for a process
// @par: none
// @par: PCB pointer    process PCB
PCB *newPCB(void) {
    // Initialize a new PCB's IO
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

    // Initialize a new PCB
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

// Prints the information about a process
// @par: PCB pointer    process PCB
// @ret: none
void printPCBInfo(PCB *process) {
    printf("PID: %d\n", process->pid);
    printf("\tPRIORITY: \t%d\n", process->priority);
    printf("\tARRIVAL TIME: \t%d\n", process->arrivalTime);
    printf("\tREMAINING TIME: %d\n", process->remainingTime);
    printf("\tSTATE: \t\t%d\n", process->state);

    printf(" IO:\tPRINTERS: \t%d / %d\n", process->IO->printersHave, process->IO->printersNeeded);
    printf("\tSCANNERS: \t%d / %d\n", process->IO->scannersHave, process->IO->scannersNeeded);
    printf("\tMODEMS: \t%d / %d\n", process->IO->modemsHave, process->IO->modemsNeeded);
    printf("\tDISKS: \t\t%d / %d\n", process->IO->disksHave, process->IO->disksNeeded);
    printf("\tMEMORY: \t%d / %d\n", process->IO->memSpaceHave, process->IO->memSpaceNeeded);
}

// Frees the memory of a process
// @par: PCB pointer    process PCB
// @ret: none
void freePCB(PCB *process) {
    // Free the PCB's actual memory
    free(process->IO);
    free(process);
}
