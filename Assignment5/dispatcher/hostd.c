#include "hostd.h"

// A process dispatcher program
// @par: int                number of arguments
// @par: pointer pointer    process list file
// @ret: int
int main(int argc, char **argv) {
    if (argc != 2) {
        // Print directions if the file is not supplied and exit
        printf("You must supply a file to the program:\n"
                "$ hostd filename.txt\n");
        exit(0);
    }

    initSystem();

    char *filename = argv[1];
    readFile(filename, dispatcher);
    if (VERBOSE) printf("Done processing %s!\n", filename);

    // Set CPU start time to 0
    int time = 0;

    // Flag when a real time process is added to the real time queue
    int rtUpdated;

    // The system is running
    while (time < runtime) {
        printf("===============================================================================================================================\n");
        printf("Time (Quantum): %d\n", time);
        printf("===============================================================================================================================\n");

        rtUpdated = updateDispatcher(time);

        if (VERBOSEMEMMAP) {
            printf("Current %d MByte memory map (each line is 64 Mbyte).\n", host.memSpaceMax);
            int memUnit;
            for(memUnit = 0; memUnit < MAX_MEMSPACE; memUnit++) {
                if (host.memSpace[memUnit] != 0) {
                    printf("%d ", host.memSpace[memUnit]);
                } else {
                    if (memUnit < MAX_RTMEMSPACE) {
                        printf(". ");
                    } else {
                        printf("_ ");
                    }
                }

                if ((memUnit + 1) % 64 == 0) {
                    printf("\n");
                }
            }
            printf("\n");
        }

        if (VERBOSEQ) {
            printf("REAL TIME QUEUE:  ");
            printQueue(realTimeQueue);

            printf("PRIORITY 1 QUEUE: ");
            printQueue(p1Queue);

            printf("PRIORITY 2 QUEUE: ");
            printQueue(p2Queue);

            printf("PRIORITY 3 QUEUE: ");
            printQueue(p3Queue);

            //TODO
            //print info about system resources and per process resources?
        }

        doProcessing(time, rtUpdated);

        time++;

        //TODO: If we hide everything else and only print the mem map, with sleep it looks amazing
        //sleep(1);
        printf("\n");
    }

    printf("===============================================================================================================================\n");
    printf("Time (Quantum): %d, program terminating\n", time);
    printf("===============================================================================================================================\n");

    closeSystem();

    return 0;
}

// Simulates one processing cycle
// @par: int    current time (quantum)
// @par: int    flag if real time queue updated
// @ret: none
void doProcessing(int time, int rtUpdated) {
    printf("------------------------------------------------------- PROCESSOR CYCLE -------------------------------------------------------\n");

    // Check if current process in a queue
    if (host.currentProcess != NULL) {
        // If current process not zero then not realtime
        if (host.currentProcess->priority != 0) {
            // If realtime queue was updated, move current process back to its queue and keep the remaining timesplice on it
            if (rtUpdated) {
                host.currentProcess->state = SUSPENDED;
                printf("* SUSPENDED pid: %d\n", host.currentProcess->pid);

                enqueueToPriority(host.currentProcess);

                // Enqueue the realtime process
                if (realTimeQueue->process != NULL) {
                    host.currentProcess = realTimeQueue->process;
                    host.currentProcess->timeSpliceLeft = host.currentProcess->remainingTime;
                    dequeueProcess(&realTimeQueue);
                }
            }
        }
    } else {
        // Processor does not have a process currently
        printf("NO PROCESS RUNNING, SEARCHING FOR ANOTHER...\n");

        if (rtUpdated) {
            // Enqueue the realtime process
            if (realTimeQueue) {
                if (realTimeQueue->process != NULL) {
                    host.currentProcess = realTimeQueue->process;
                    host.currentProcess->timeSpliceLeft = host.currentProcess->remainingTime;
                    dequeueProcess(&realTimeQueue);
                }
            }
        } else if (p1Queue) {
            if (p1Queue->process != NULL) {
                host.currentProcess = p1Queue->process;
                dequeueProcess(&p1Queue);
                host.currentProcess->timeSpliceLeft = P1_TIMEQUANTUM;
            }
        } else if (p2Queue) {
            if (p2Queue->process != NULL) {
                host.currentProcess = p2Queue->process;
                dequeueProcess(&p2Queue);
                host.currentProcess->timeSpliceLeft = P2_TIMEQUANTUM;
            }
        } else if (p3Queue) {
            if (p3Queue->process != NULL) {
                host.currentProcess = p3Queue->process;
                dequeueProcess(&p3Queue);
                host.currentProcess->timeSpliceLeft = P3_TIMEQUANTUM;
            }
        } else {
            return;
        }
    }

    // Actual processing goes here
    if (host.currentProcess != NULL) {
        host.currentProcess->state = RUNNING;
        host.currentProcess->timeSpliceLeft--;
        host.currentProcess->remainingTime--;
        printf("* RUNNING pid: %d\n", host.currentProcess->pid);

        if (VERBOSE) printf("Time splice left in this queue: %d, total remaining time: %d\n", host.currentProcess->timeSpliceLeft, host.currentProcess->remainingTime);

        if (host.currentProcess->remainingTime == 0) {
            // Process has no time splice left in this queue, terminate the process
            freeHostResources(host.currentProcess);
            host.currentProcess->state = TERMINATED;
            printf("* TERMINATED pid: %d, process completed!\n", host.currentProcess->pid);
            free(host.currentProcess);
            host.currentProcess = NULL;
        } else if (host.currentProcess->timeSpliceLeft == 0) {
            // Round robin queuing
            if (host.currentProcess->priority < 3 && host.currentProcess->priority >= 1) {
                host.currentProcess->priority++;
                enqueueToPriority(host.currentProcess);
                host.currentProcess = NULL;
            } else if (host.currentProcess->priority >= 3) {
                host.currentProcess->timeSpliceLeft = P3_TIMEQUANTUM;
                host.currentProcess->priority = 1;
                enqueueToPriority(host.currentProcess);
                host.currentProcess = NULL;
            }
        }
    }
}

// Updates the dispatcher
// @par: int    current time (quantum)
// @ret: int    1 for process put into real time queue
// @ret: int    0 otherwise
int updateDispatcher(int time) {
    // Create a pointer to the dispatcher so we don't modify dispatcher directly unless we need to
    Queue *queuePtr = dispatcher;

    // Create a new queue for elements that weren't able to be added
    Queue *newDispatcher = initializeQueue();

    int rtQUpdated = 0;

    // Checks if dispatcher is empty
    if (!(queuePtr)) {
        printf("The queue is empty!\n\n");
        return 0;
    }

    // Iterate through each element in the queue
    int i = 0;
    int count = numElems(queuePtr);

    // If the first process is NULL, we don't want to iterate through them
    // Double check that there are elements in queue
    if (queuePtr->process == NULL) {
        i = count;
    }

    // Keep an indicator to see if anything breaks
    int broken = 0;

    // Iterate through each element in the dispatcher
    while (i < count) {
        // Dequeue the next process from the queue
        PCB *currentProcess = dequeueProcess(&queuePtr)->process;

        // Queue is empty
        // Triple check
        if (currentProcess == NULL) {
            printf("BROKEN!\n");
            broken = 1;
            break;
        }

        // If the process has "arrived", attempt to allocate resources to it
        if (time >= currentProcess->arrivalTime) {
            // Attempt to allocate resources to the process.
            int allocResult = allocateResources(currentProcess);

            if (allocResult) {
                // Resources were allocated successfully
                printf("* INITIALIZED pid: %d", currentProcess->pid);
                if (currentProcess->priority == 0) {
                    rtQUpdated = 1;
                    printf(", a real time process");
                }
                printf("\n\n");

                // Send the process to join the appropriate priority queue
                enqueueToPriority(currentProcess);
            } else {
                // Resources could not be allocated, move the process back on the dispatcher queue
                enqueueProcess(newDispatcher, currentProcess);
            }
        } else {
            // The time has not come for this process, add it to the new dispatcher
            enqueueProcess(newDispatcher, currentProcess);
        }

        i++;
    }

    // If the queue wasn't broken
    if (!broken) {
        if (dispatcher == NULL) {
            cleanQueue(dispatcher);
        }
        dispatcher = newDispatcher;
    }

    return rtQUpdated;
}

// Updates the dispatcher
// @par: PCB pointer    process PCB
// @ret: none
void enqueueToPriority(PCB *process) {
    int p = process->priority;

    if (p == 0) {
        if (!realTimeQueue) {
            realTimeQueue = initializeQueue();
        }
        enqueueProcess(realTimeQueue, process);
    } else if (p == 1) {
        if (!p1Queue) {
            p1Queue = initializeQueue();
        }
        enqueueProcess(p1Queue, process);
    } else if (p == 2) {
        if (!p2Queue) {
            p2Queue = initializeQueue();
        }
        enqueueProcess(p2Queue, process);
    } else {
        if (!p3Queue) {
            p3Queue = initializeQueue();
        }
        enqueueProcess(p3Queue, process);
    }
}

// Initializes the system (global variables)
// @par: none
// @ret: none
void initSystem(void) {
    host.numPrinters = MAX_PRINTERS;
    host.numScanners = MAX_SCANNERS;
    host.numModems = MAX_MODEMS;
    host.numDisks = MAX_DISKS;
    host.memSpaceMax = MAX_MEMSPACE;
    host.rtMemSpaceMax = MAX_RTMEMSPACE;
    host.rtMemSpaceUsed = 0;
    host.currentProcess = NULL;

    memset(host.printersAlloc, 0, MAX_PRINTERS);
    memset(host.scannersAlloc, 0, MAX_SCANNERS);
    memset(host.modemsAlloc, 0, MAX_MODEMS);
    memset(host.disksAlloc, 0, MAX_DISKS);
    memset(host.memSpace, 0, MAX_MEMSPACE);

    // Initialize all queues to be separate queues
    dispatcher = initializeQueue();
    realTimeQueue = initializeQueue();
    userJobQueue = initializeQueue();
    p1Queue = initializeQueue();
    p2Queue = initializeQueue();
    p3Queue = initializeQueue();
}

// Shuts down the system (global variables) by freeing memory
// @par: none
// @ret: none
void closeSystem(void) {
    //TODO anything else here?
    /*reset resource counters?*/
    /*reset memory limit?*/

    // Empty all queues
    cleanQueue(dispatcher);
}
