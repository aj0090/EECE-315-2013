#ifndef MONITOR_H
#define MONITOR_H

// Includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Constants

// Define VERBOSE to 1 for debugging outputs
#define VERBOSE 0

// Struct definitions

// Function Prototypes
void readCPUInfo(void);
void readKernelVersion(void);
void readUptime(void);

#endif
