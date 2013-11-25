#ifndef PROCESSES_H
#define PROCESSES_H

// Includes
//#include <stdio.h>

// Constants
#define MAX_PRINTERS 2
#define MAX_SCANNERS 1
#define MAX_MODEMS 1
#define MAX_DISKS 2

// Struct definitions
typedef struct pcb {
    int pid;
    int priority;
    int memory;
    int arrivalTime;
    int remainingTime;
    struct pcbStatus *status;
    struct pcbResources *io;
} PCB;

typedef struct pcbStatus {
    int started;
    int running;
    int suspended;
    int terminated;
} PBCStatus;

//TODO: should this define the process's resources or the system's resources?
typedef struct systemResources {
    int printer;
    int scanner;
    int modem;
    int disk;
} SYSResources;

// Function Prototypes
void function(void);

#endif
