#ifndef JOBS_H
#define JOBS_H

// Includes
#include <stdio.h>

// Constants
#define THIS    0
#define MAX_PRINTERS 2
#define MAX_SCANNERS 1
#define MAX_MODEMS 1
#define MAX_DISKS 2

// Define VERBOSE to 1 for debugging outputs
#define VERBOSE 0

// Struct definitions
typedef struct pcb {
    int pid;
    int priority;
    int memory;
    int arrivalTime;
    int remainingTime;
    struct pcbStatus *status;
    struct pcbResources *io;
};

typedef struct pcbStatus {
    int started;
    int running;
    int suspended;
    int terminated;
}

//TODO: should this define the process's resources or the system's resources?
typedef struct pcbResources {
    int printer;
    int scanner;
    int modem;
    int disk;
};

// Function Prototypes
void function(void);

#endif
