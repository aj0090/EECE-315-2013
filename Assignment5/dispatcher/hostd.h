#ifndef HOSTD_H
#define HOSTD_H

// Includes
//#include <stdio.h>
//#include <stdlib.h>
//#include <unistd.h>
//#include <signal.h>
//#include <sys/types.h>
#include <string.h>
#include "processes/processes.h"
#include "queue/queue.h"

// Constants
#define MAX_PROCESSES 1000
#define FILE_BUFFER 100

// Define VERBOSE to 1 for debugging outputs
#define VERBOSE 1

// Global variables
int cpuTime;
Queue *dispatcher, *realTimeQueue, *userJobQueue, *p1Queue, *p2Queue, *p3Queue;

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
