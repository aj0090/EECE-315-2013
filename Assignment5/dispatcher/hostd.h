#ifndef HOSTD_H
#define HOSTD_H

// Includes
//#include <stdio.h>
//#include <stdlib.h>
//#include <unistd.h>
//#include <signal.h>
//#include <sys/types.h>
#include "processes/processes.h"
#include "queue/queue.h"

// Constants
#define MAX_PROCESSES 100
#define FILE_BUFFER 100

// Define VERBOSE to 1 for debugging outputs
#define VERBOSE 0

// Global variables
int cpuTime;
Queue *dispatcher, *realTimeQueue, *userJobQueue, *p1Queue, *p2Queue, *p3Queue;

// Function prototypes
void readFile(char *filename, Queue *dispatcher);
void printPCBInfo(PCB *process);
int *stringToInts(char *string);


#endif
