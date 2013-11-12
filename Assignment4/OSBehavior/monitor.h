#ifndef MONITOR_H
#define MONITOR_H

// Includes
#include "headers.h"
#include "utils.h"
#include <time.h>

// Constants

// Struct definitions

// Function Prototypes
int readCPUInfo(void);
void readKernelVersion(void);
int readUptime(char *val, int numProc);
void readStat(int numProc);
void readDiskStats();
void getContextSwitches();
void getBootTime();
void getProcessesCreated();

#endif
