#ifndef MONITOR_H
#define MONITOR_H

// Includes
#include "headers.h"
#include "utils.h"

// Constants

// Struct definitions

// Function Prototypes
int readCPUInfo(void);
void readKernelVersion(void);
void readUptime();
void readStat(int numProc);
void readDiskStats();
void getContextSwitches();
void getBootTime();
void getProcessesCreated();
void readMemInfo();
void readLoadAvg(double sleepTime, double runTime);

#endif
