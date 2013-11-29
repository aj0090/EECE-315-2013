#ifndef PCB_H
#define PCB_H

// Includes

// Constants
#define MAX_PRINTERS 2
#define MAX_SCANNERS 1
#define MAX_MODEMS 1
#define MAX_DISKS 2
#define MAX_MEMSPACE 1024
#define MAX_RTMEMSPACE 96

// Define states
// TODO: we should prepend the states with STATE_
#define STARTED 0
#define RUNNING 1
#define SUSPENDED 2
#define TERMINATED 3

// Define time quantum
#define P1_TIMEQUANTUM 1
#define P2_TIMEQUANTUM 2
#define P3_TIMEQUANTUM 3

// Struct definitions
// A process's resources
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

// A process
typedef struct pcb {
    int pid;
    int priority;
    int arrivalTime;
    int remainingTime;

    // TODO: difference between this and remainingTime?
    int timeSpliceLeft;

    int state;

    struct PCBIO *IO;
} PCB;

// The host
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

#endif
