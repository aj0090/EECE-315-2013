#ifndef HOSTD_H
#define HOSTD_H

// Includes
//#include <stdio.h>
//#include <stdlib.h>
//#include <signal.h>
//#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include "resources/pcb.h"
#include "queue/queue.h"

// Constants
#define MAX_PROCESSES 1000
#define FILE_BUFFER 100

// Define for general debugging outputs (eg pcb, time slices)
#define VERBOSE 1
// DEFINE for queue information
#define VERBOSEQ 1
// Define for memory maps
#define VERBOSEMEMMAP 1
// Define for resource info
#define VERBOSER 1

// Global variables
Queue *dispatcher, *realTimeQueue, *userJobQueue, *p1Queue, *p2Queue, *p3Queue;
int runtime;

// Function prototypes
void freeHostResources(PCB *process);

void doProcessing(int time, int rtUpdated);
int updateDispatcher(int time);

void enqueueToPriority(PCB *process);
int checkResourcesFreeFirstFit(int numResourcesReq, int pid, int numHostResources, int *hostResources);
int allocatePrinters(PCB *process);
int allocateScanners(PCB *process);
int allocateModems(PCB *process);
int allocateDisks(PCB *process);
int allocateRealTimeMemory(PCB *process);
int allocateMemory(PCB *process);

int allocateResources(PCB *process);
void readFile(char *filename, Queue *dispatcher);
void printPCBInfo(PCB *process);
int *stringToInts(char *string);

void initSystem(void);
void closeSystem(void);
void freePCB(PCB *);
PCB *newPCB(void);

#endif
