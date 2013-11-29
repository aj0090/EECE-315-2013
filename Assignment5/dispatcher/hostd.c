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

    int time = 0;
    //TODO: what does this rep? that the rtq has been updated or that a new process has been added to it?
    int rtUpdated;

    // The system is running
    while (time < 30) {
        printf("===============================================================================================================================\n");
        printf("Time (Quantum): %d\n", time);
        printf("===============================================================================================================================\n");

        //TODO: what does this rep? that the rtq has been updated or that a new process has been added to it?
        rtUpdated = updateDispatcher(time);

        if (VVERBOSE) {
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

            printf("REAL TIME QUEUE:  ");
            printQueue(realTimeQueue);

            printf("PRIORITY 1 QUEUE: ");
            printQueue(p1Queue);

            printf("PRIORITY 2 QUEUE: ");
            printQueue(p2Queue);

            printf("PRIORITY 3 QUEUE: ");
            printQueue(p3Queue);


            printf("PRINT SOME INFO ABOUT SYSTEM RESOURCES AT EACH STAGE?\n");
            printf("print info about current process at each stage?\n");
        }

        doProcessing(time, rtUpdated);

        time++;

        //TODO: If we hide everything else and only print the mem map, with sleep it looks amazing
        //sleep(1);
    }

    printf("===============================================================================================================================\n");
    printf("Time (Quantum): %d, program terminating\n", time);
    printf("===============================================================================================================================\n");

    closeSystem();

    return 0;
}








//TODO need to work on this section below


// Simulates one processing cycle
// @par: int    current time (quantum)
// @par: int    ??? the status of the rtq (if a process is in it?)
// @ret: none
void doProcessing(int time, int rtUpdated)
{
    // TODO DEBUG, still need this?
    // Switching processes goes here
    /*
       printf("HELLO!\n");

       if (host.currentProcess == NULL && time >= 12) {
       printf("current process is null.\n");
       host.currentProcess = p1Queue->process;
       dequeueProcess(&p1Queue);
       }
       if (host.currentProcess != NULL) {
       printf("host printers: %d\n", host.numPrinters);
       printf("current printer: %d\n", host.currentProcess->IO->printerStartID);
       freeHostResources(host.currentProcess);
       }
       */
    // REPLACEMENT CHECK

    printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++ PROCESSOR CYCLE +++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");

    // TODO: i just need to clearly understand what this represents and it's all good
    printf("rtUpdated: %d\n", rtUpdated);

    // TODO: is this just checking & prepping the queues? (aka checking for realtime processes?)
    if (host.currentProcess != NULL) {
        // If current process not zero then not realtime
        if (host.currentProcess->priority != 0) {
            // If realtime queue was updated, move current process back to its queue and keep the remaining timesplice on it
            if (rtUpdated) {
                host.currentProcess->state = SUSPENDED;
                printf("SUSPENDED pid %d\n", host.currentProcess->pid);

                enqueueToPriority(host.currentProcess);

                // Enqueue the realtime process
                // TODO: should we print anything?
                if (realTimeQueue->process != NULL) {
                    host.currentProcess = realTimeQueue->process;
                    host.currentProcess->timeSpliceLeft = host.currentProcess->remainingTime;
                    dequeueProcess(&realTimeQueue);
                }
            }
        }
    } else {
        // Processor does not have a process currently
        printf("NO PROCESS QUEUED, SEARCHING...\n");

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
        printf("RUNNING pid: %d\n", host.currentProcess->pid);

        //TODO: what is the difference between time splice left and remaining time? Especially look at next comment
        printf("Time splice left: %d, remaining time: %d\n", host.currentProcess->timeSpliceLeft, host.currentProcess->remainingTime);

        // Process has no time splice left
        if (host.currentProcess->remainingTime == 0) {
            freeHostResources(host.currentProcess);
            host.currentProcess->state = TERMINATED;
            printf("TERMINATED pid: %d, process completed!\n", host.currentProcess->pid);
            free(host.currentProcess);
            host.currentProcess = NULL;
        } else if (host.currentProcess->timeSpliceLeft == 0) {
            //
            //TODO: i think if we clear this up we can get rid of the "WHAT"
            // is this when the process gets bumped down a queue priority?
            printf("WHAT\n");
            if (host.currentProcess->priority < 3 && host.currentProcess->priority >= 1) {
                //TODO
                printf("huh.\n");
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
    // TODO Process termination/transcending of queues goes here
}



// Returns 1 if theres a new process placed in the Realtime Queue,
// else returns 0
int updateDispatcher(int time)
{
    // Create a pointer to the dispatcher so we don't modify dispatcher directly unless we need to
    Queue *queuePtr = dispatcher;

    // Create a new queue for elements that weren't able to be added.
    Queue *newDispatcher = initializeQueue();
    int rtQUpdated = 0;

    if (!(queuePtr)) {
        printf("The queue is empty!\n\n");
        return 0;
    }

    // Iterate through each element in the queue
    int i = 0;
    int count = numElems(queuePtr);

    // If the first process is NULL, we don't want to iterate through them
    // (double check that there are elements in queue)
    if (queuePtr->process == NULL)
        i = count;

    // Keep an indicator to see if anything breaks
    int broken = 0;
    //printf("Num elems: %d\n", count);

    // Iterate through each element in the dispatcher
    while (i < count) {


        //printf("%d/%d\n", i + 1, count);
        //dequeueProcess(&queuePtr);

        // Dequeue the next process from the queue
        PCB *currentProcess = dequeueProcess(&queuePtr)->process;

        // Queue is empty.
        // (Triple check)
        if (currentProcess == NULL) {
            printf("BROKEN!\n");
            broken = 1;
            break;
        }

        // If the process has "arrived", attempt to allocate resources to it
        if (time >= currentProcess->arrivalTime) {
            //printf("HELLO from PID: %d!\n", currentProcess->pid);
            // Attempt to allocate resources to the process.
            int allocResult = allocateResources(currentProcess);

            // Resources were allocated successfully
            if (allocResult) {
                // Send the process to join the appropriate priority queue
                printf("Successful add!\n");
                enqueueToPriority(currentProcess);
                if (currentProcess->priority == 0) {
                    rtQUpdated = 1;
                    printf("RT: %d\n", rtQUpdated);
                }
                printf("\n\n");
            } else {
                // Resources could not be allocated for it, throw the process back on the dispatcher queue
                enqueueProcess(newDispatcher, currentProcess);
            }
        } else {
        // The time has not come for this process, add it to the new dispatcher.
            enqueueProcess(newDispatcher, currentProcess);
        }
        i++;
    }
    // If the queue wasn't broken,
    if (!broken) {
        if (dispatcher == NULL) {
            cleanQueue(dispatcher);
        }
        dispatcher = newDispatcher;
    }
    //printf("here?\n");
    return rtQUpdated;
}


void enqueueToPriority(PCB *process)
{
    int p = process->priority;
    if (p == 0)
    {
        if (!realTimeQueue)
            realTimeQueue = initializeQueue();
        enqueueProcess(realTimeQueue, process);
    }
    else if (p == 1)
    {
        if (!p1Queue)
            p1Queue = initializeQueue();
        enqueueProcess(p1Queue, process);
    }
    else if (p == 2)
    {
        if (!p2Queue)
            p2Queue = initializeQueue();
        enqueueProcess(p2Queue, process);
    }
    else
    {
        if (!p3Queue)
            p3Queue = initializeQueue();
        enqueueProcess(p3Queue, process);
    }

    return;
}
















// Initializes the system (global variables)
// @par: none
// @ret: none
void initSystem(void) {
    /*set resource counters*/
    /*set memory limit*/

    // TODO: what is this i used for? vague name for global variable
    int i;

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

    // Set CPU start time to 0
    cpuTime = 0;
}

// Shuts down the system (global variables) by freeing memory
// @par: none
// @ret: none
void closeSystem(void) {
    /*reset resource counters?*/
    /*reset memory limit?*/

    // Empty all queues
    cleanQueue(dispatcher);
}
