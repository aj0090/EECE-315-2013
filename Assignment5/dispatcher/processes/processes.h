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
    struct resources *io;
} PCB;

typedef struct pcbStatus {
    int started;
    int running;
    int suspended;
    int terminated;
} PCBStatus;

//TODO: this define the process's resources, but also stands for the system's resources as a global var?
typedef struct resources {
    int printer;
    int scanner;
    int modem;
    int disk;
} PCBIO;

// Global variables
struct resources SYSIO;

// Function prototypes
void function(void);

#endif
