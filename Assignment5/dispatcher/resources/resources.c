#include "../hostd.h"

// Allocates resources to a process
// @par: PCB pointer    process PCB
// @ret: int            2 if hostd contains enough resources & process is realtime
// @ret: int            1 if hostd contains enough resources
// @ret: int            0 otherwise
int allocateResources(PCB *process) {
    int printers, scanners, modems, disks, memory;

    //TODO
    //printf("Printer Allocated: %d\n", allocatePrinters(process));
    printers = allocatePrinters(process);
    scanners = allocateScanners(process);
    modems = allocateModems(process);
    disks = allocateDisks(process);

    int realTime = (process->priority == 0);

    if (realTime) {
        memory = allocateRealTimeMemory(process);
    } else {
        memory = allocateMemory(process);
    }

    if (printers && scanners && modems && disks && memory) {
        // If every resource was allocated (1s)
        //TODO make sure this isn't broken after changing from ret -1 to ret 0
        return realTime ? 2 : 1;
    } else {
        printf("Not enough resources, freeing resources!\n");
        freeHostResources(process);
        return 0;
    }
}

// Allocates printers to a process
// @par: PCB pointer    process PCB
// @ret: int            1 successful allocation
// @ret: int            0 otherwise
int allocatePrinters(PCB *process) {
    // No allocation needed, done
    if (process->IO->printersNeeded == 0) {
        return 1;
    }

    int startCursor = checkResourcesFreeFirstFit(process->IO->printersNeeded, process->pid, host.numPrinters, host.printersAlloc);
    int i;

    // Adequate resources are available starting at startCursor
    if (startCursor != -1) {
        // Allocate the host's printers to the process PID
        for(i = startCursor; i < startCursor + process->IO->printersNeeded; i++) {
            host.printersAlloc[i] = process->pid;
        }

        // Update the process's printers requirements
        process->IO->printersHave = process->IO->printersNeeded;
        printf("Allocated %d printers starting at printer #%d for pid %d.\n", process->IO->printersHave, startCursor, process->pid);
        process->IO->printerStartID = startCursor;
        return 1;
    }

    printf("Could not allocate printers for pid %d\n", process->pid);
    return 0;
}

// Allocates scanners to a process
// @par: PCB pointer    process PCB
// @ret: int            1 successful allocation
// @ret: int            0 otherwise
int allocateScanners(PCB *process) {
    // No allocation needed, done
    if (process->IO->scannersNeeded == 0) {
        return 1;
    }

    int startCursor = checkResourcesFreeFirstFit(process->IO->scannersNeeded, process->pid, host.numScanners, host.scannersAlloc);
    int i;

    // Adequate resources are available starting at startCursor
    if (startCursor != -1) {
        // Allocate the host's scanners to the process PID
        for(i = startCursor; i < startCursor + process->IO->scannersNeeded; i++) {
            host.scannersAlloc[i] = process->pid;
        }

        // Update the process's scanner requirements
        process->IO->scannersHave = process->IO->scannersNeeded;
        printf("Allocated %d scanners starting at scanner #%d for pid %d.\n", process->IO->scannersHave, startCursor, process->pid);
        process->IO->scannerStartID = startCursor;
        return 1;
    }

    printf("Could not allocate scanners for pid %d\n", process->pid);
    return 0;
}

// Allocates modems to a process
// @par: PCB pointer    process PCB
// @ret: int            1 successful allocation
// @ret: int            0 otherwise
int allocateModems(PCB *process) {
    // No allocation needed, done
    if (process->IO->modemsNeeded == 0) {
        return 1;
    }

    int startCursor = checkResourcesFreeFirstFit(process->IO->modemsNeeded, process->pid, host.numModems, host.modemsAlloc);
    int i;

    // Adequate resources are available starting at startCursor
    if (startCursor != -1) {
        // Allocate the host's modems to the process PID
        for(i = startCursor; i < startCursor + process->IO->modemsNeeded; i++) {
            host.modemsAlloc[i] = process->pid;
        }

        // Update the process's modem requirements
        process->IO->modemsHave = process->IO->modemsNeeded;
        printf("Allocated %d modems starting at modem #%d for pid %d.\n", process->IO->modemsHave, startCursor, process->pid);
        process->IO->modemStartID = startCursor;
        return 1;
    }

    printf("Could not allocate modems for pid %d\n", process->pid);
    return 0;
}

// Allocates disks to a process
// @par: PCB pointer    process PCB
// @ret: int            1 successful allocation
// @ret: int            0 otherwise
int allocateDisks(PCB *process) {
    // No allocation needed, done
    if (process->IO->disksNeeded == 0) {
        return 1;
    }

    int startCursor = checkResourcesFreeFirstFit(process->IO->disksNeeded, process->pid, host.numDisks, host.disksAlloc);
    int i;

    // Adequate resources are available starting at startCursor
    if (startCursor != -1) {
        // Allocate the host's disks to the process PID
        for(i = startCursor; i < startCursor + process->IO->disksNeeded; i++) {
            host.disksAlloc[i] = process->pid;
        }

        // Update the process's modem requirements
        process->IO->disksHave = process->IO->disksNeeded;
        printf("Allocated %d disks starting at disk #%d for pid %d.\n", process->IO->disksHave, startCursor, process->pid);
        process->IO->diskStartID = startCursor;
        return 1;
    }

    printf("Could not allocate disks for pid %d\n", process->pid);
    return 0;
}

// Allocates memory to a real time process
// @par: PCB pointer    process PCB
// @ret: int            1 successful allocation
// @ret: int            0 otherwise
int allocateRealTimeMemory(PCB *process) {
    // No allocation needed, done
    if (process->IO->memSpaceNeeded == 0) {
        return 1;
    }

    // Memory array, but only let it see the first 96 bytes (since this is is a real time process)
    int startCursor = checkResourcesFreeFirstFit(process->IO->memSpaceNeeded, process->pid, host.rtMemSpaceMax, host.memSpace);
    int i;

    // Adequate resources are available starting at startCursor
    if (startCursor != -1) {
        // Allocate the host's memory to the process PID
        for(i = startCursor; i < startCursor + process->IO-> memSpaceNeeded; i++) {
            host.memSpace[i] = process->pid;
        }

        // Update the process's memory requirements
        process->IO->memSpaceHave = process->IO->memSpaceNeeded;
        printf("Allocated %d MByte's starting at address #%d for pid %d.\n", process->IO->memSpaceHave, startCursor, process->pid);
        process->IO->memSpaceStartID = startCursor;
        return 1;
    }

    printf("Could not allocate real time memory for pid %d\n", process->pid);
    return 0;
}

// Allocates memory to a real time process
// @par: PCB pointer    process PCB
// @ret: int            1 successful allocation
// @ret: int            0 otherwise
int allocateMemory(PCB *process) {
    // No allocation needed, done
    if (process->IO->memSpaceNeeded == 0) {
        return 1;
    }

    // Memory array, but only let it see after first 96 bytes (ignore real time memory)
    // Also show that the length is 96 bytes less real time memory
    int startCursor = checkResourcesFreeFirstFit(process->IO->memSpaceNeeded, process->pid, host.memSpaceMax - host.rtMemSpaceMax, host.memSpace + host.rtMemSpaceMax);
    int i;

    // Adequate resources are available starting at startCursor
    if (startCursor != -1) {
        // Allocate the host's memory to the process PID
        for(i = startCursor + host.rtMemSpaceMax; i < startCursor + process->IO->memSpaceNeeded + host.rtMemSpaceMax; i++) {
            host.memSpace[i] = process->pid;
        }

        // Update the process's memory requirements
        process->IO->memSpaceHave = process->IO->memSpaceNeeded;
        printf("Allocated %d MByte's starting at address #%d for pid %d.\n", process->IO->memSpaceHave, startCursor + host.rtMemSpaceMax, process->pid);
        process->IO->memSpaceStartID = startCursor + host.rtMemSpaceMax;
        return 1;
    }

    printf("Could not allocate memory for pid %d\n", process->pid);
    return 0;
}






//TODO work on this section below

// Frees a process's resources from the host
// @par: PCB pointer    process PCB
// @ret: none
void freeHostResources(PCB *process) {
    //TODO I don't think this i has to be init to 0
    int i = 0;

    printf("Freeing pid %d's resources:", process->pid);

    //TODO add prints for all resources properly

    //TODO this prints all of host's printers, allocated or not
    /*printf("Host printers: %d\n", host.numPrinters);*/
    //TODO this only prints the first resource!
    /*printf("Printers: %d/%d\n", process->IO->printerStartID, host.numPrinters);*/

    if (process->IO->printerStartID != -1) {
        for(i = process->IO->printerStartID; i < process->IO->printerStartID + process->IO->printersHave ; i++) {
            host.printersAlloc[i] = 0;
        }
        printf(" printers");
    }

    if (process->IO->scannerStartID != -1) {
        for(i = process->IO->scannerStartID; i < (process->IO->scannerStartID + process->IO->scannersHave); i++) {
            host.scannersAlloc[i] = 0;
        }
        printf(" scanners");
    }

    if (process->IO->modemStartID != -1) {
        for(i = process->IO->modemStartID; i < (process->IO->modemStartID + process->IO->modemsHave); i++) {
            host.modemsAlloc[i] = 0;
        }
        printf(" modems");
    }

    if (process->IO->diskStartID != -1) {
        for(i = process->IO->diskStartID; i < (process->IO->diskStartID + process->IO->disksHave); i++) {
            host.disksAlloc[i] = 0;
        }
        printf(" disks");
    }

    if (process->IO->memSpaceStartID != -1) {
        for(i = process->IO->memSpaceStartID; i < (process->IO->memSpaceStartID + process->IO->memSpaceHave); i++) {
            host.memSpace[i] = 0;
        }
        printf(" memory");
    }

    printf("... Ok!\n");
    /*printf("Freed pid %d's resources!\n", process->pid);*/
}

// Returns the index where resources can be allocated based on first fit
// @par: int            amount of resources requested
// @par: int            pid
// @par: int            total amount of host resources
// @par: int pointer    host resources
// @ret: int            index for allocation, -1 if no resources available
int checkResourcesFreeFirstFit(int numResourcesReq, int pid, int numHostResources, int *hostResources) {
    int i;
    int cursor = -1, count = 0;

    for(i = 0; i < numHostResources; i++) {
        // 0 signifies no process is using it
        if (hostResources[i] == 0) {
            // If the last element wasn't free, set the printerCursor to the current element
            if (cursor == -1) {
                cursor = i;
            }

            count++;
            // Check if the desired resource count was met
            if (count == numResourcesReq) {
                break;
            }
        } else if (hostResources[i] == pid) {
            // This shouldn't happen in the current version, but if resource is already allocated, error!
            return -1;
        } else {
            // TODO: clarification? Allocated memory block found.
            // Set the current count to zero and make the printerCursor = -1
            cursor = -1;
            count = 0;
        }
    }

    if (count == numResourcesReq) {
        // Enough resources were found
        return cursor;
    } else {
        // Not enough resources were found
        return -1;
    }
}
