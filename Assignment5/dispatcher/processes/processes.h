#ifndef PROCESSES_H
#define PROCESSES_H

// Includes
//#include <stdio.h>

// Constants
#define MAX_PRINTERS 2
#define MAX_SCANNERS 1
#define MAX_MODEMS 1
#define MAX_DISKS 2
#define MAX_MEMSPACE 1024
#define MAX_RTMEMSPACE 96

// Defining states
#define STARTED 0
#define RUNNING 1
#define SUSPENDED 2
#define TERMINATED 3


// Defining time quantum
#define P1_TIMEQUANTUM 1
#define P2_TIMEQUANTUM 2
#define P3_TIMEQUANTUM 3


//TODO: this define the process's resources, but also stands for the system's resources as a global var?
typedef struct PCBIO {
    int printersNeeded;
    int printersHave;
    int printerStartID;

    int scannersNeeded; 
    int scannersHave;
    int scannerStartID;

    int modemsNeeded;
    int modemsHave;
    int modemStartID;

    int disksNeeded;
    int disksHave;
    int diskStartID;

    int memSpaceNeeded;
    int memSpaceHave;
    int memSpaceStartID;
} PCBIO;

// Struct definitions
typedef struct pcb {
    int pid;
    int priority;
    int arrivalTime;
    int remainingTime;

    int timeSpliceLeft;

    int state;

    struct PCBIO *IO;
} PCB;

typedef struct host {
    int numPrinters;
    int printersAlloc[MAX_PRINTERS];

    int numScanners;
    int scannersAlloc[MAX_SCANNERS];

    int numModems;
    int modemsAlloc[MAX_MODEMS];
   
    int numDisks;
    int disksAlloc[MAX_DISKS];

    int memSpaceMax;
    int memSpace[MAX_MEMSPACE];
    int rtMemSpaceMax;
    int rtMemSpaceUsed;

    PCB *currentProcess;

} HOSTResources;

// Global variables
HOSTResources host;



/*
typedef struct pcbStatus {
    int started;
    int running;
    int suspended;
    int terminated;
} PCBStatus;
*/


// Function prototypes
void function(void);

#endif
