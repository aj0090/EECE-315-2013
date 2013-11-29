#include "hostd.h"

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
    // TODO: what is rtUpdated? That a realtime process has been added?
    int rtUpdated;

    // The system is running
    while (time < 25) {
        printf("===============================================================================================================================\n");
        printf("Time (Quantum): %d\n", time);
        printf("===============================================================================================================================\n");

        //TODO: what does this rep? that the rtq has been updated or that a new process has been added?
        rtUpdated = updateDispatcher(time);

        if (VVERBOSE) {
            printf("Current %d MByte memory map (each line is 64 Mbyte).\n", host.memSpaceMax);
            int memUnit;
            int memBlock;
            for(memUnit = 0; memUnit < host.memSpaceMax; memUnit++) {
                if (host.memSpace[memUnit] != 0) {
                    printf("%d ", host.memSpace[memUnit]);
                } else {
                    printf("_ ");
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








//TODO work on this section below


void doProcessing(int time, int rtUpdated)
{
    // Switching processes goes here
    /*
       printf("HELLO!\n");

       if(host.currentProcess == NULL && time >= 12){
       printf("current process is null.\n");
       host.currentProcess = p1Queue->process;
       dequeueProcess(&p1Queue);
       }
       if(host.currentProcess != NULL){
       printf("host printers: %d\n", host.numPrinters);
       printf("current printer: %d\n", host.currentProcess->IO->printerStartID);
       freeHostResources(host.currentProcess);
       }
       */
    // REPLACEMENT CHECK

    // TODO: analyze this section...
    printf("\n\nIN PROCESSOR\n");


    printf("rtUpdated: %d\n", rtUpdated);
    if(host.currentProcess != NULL){

        // Current process priority is not zero, therefore not realtime
        if(host.currentProcess->priority != 0){

            // If realtime processes were updated
            if(rtUpdated)
            {
                // Move the process back to its queue, and keep the remaining timesplice on it
                host.currentProcess->state = SUSPENDED;
                enqueueToPriority(host.currentProcess);

                // Enqueue the realtime process
                if(realTimeQueue->process != NULL)
                {
                    host.currentProcess = realTimeQueue->process;
                    host.currentProcess->timeSpliceLeft = host.currentProcess->remainingTime;
                    dequeueProcess(&realTimeQueue);
                }
            }

        }

    }
    // Processor does not have a process currently
    else{
        printf("NO PROCESS, LETS FIND ONE\n");
        if(rtUpdated){
            // Enqueue the realtime process
            if(realTimeQueue){
                if(realTimeQueue->process != NULL)
                {
                    host.currentProcess = realTimeQueue->process;
                    host.currentProcess->timeSpliceLeft = host.currentProcess->remainingTime;
                    dequeueProcess(&realTimeQueue);

                }
            }
        }
        else if (p1Queue) {
            if(p1Queue->process != NULL)
            {
                host.currentProcess = p1Queue->process;
                dequeueProcess(&p1Queue);
                host.currentProcess->timeSpliceLeft = P1_TIMEQUANTUM;
            }
        }
        else if(p2Queue) {
            if(p2Queue->process != NULL)
            {
                host.currentProcess = p2Queue->process;
                dequeueProcess(&p2Queue);
                host.currentProcess->timeSpliceLeft = P2_TIMEQUANTUM;
            }
        }
        else if(p3Queue) {
            if(p3Queue->process != NULL)
            {
                host.currentProcess = p3Queue->process;
                dequeueProcess(&p3Queue);
                host.currentProcess->timeSpliceLeft = P3_TIMEQUANTUM;
            }
        }
        else{
            return;
        }

    }

    // Actual processing goes here

    if(host.currentProcess != NULL){
        host.currentProcess->state = RUNNING;
        host.currentProcess->timeSpliceLeft--;
        host.currentProcess->remainingTime--;
        printf("PID: %d\n", host.currentProcess->pid);
        printf("Time splice left: %d, remaining time: %d\n", host.currentProcess->timeSpliceLeft, host.currentProcess->remainingTime);

        // Process has no time splice left
        if(host.currentProcess->remainingTime == 0)
        {
            printf("Process finished running!\n");
            freeHostResources(host.currentProcess);
            host.currentProcess->state = TERMINATED;
            free(host.currentProcess);
            host.currentProcess = NULL;
        }
        else if(host.currentProcess->timeSpliceLeft == 0)
        {
            printf("WHAT\n");
            if(host.currentProcess->priority < 3 && host.currentProcess->priority >= 1)
            {
                printf("huh.\n");
                host.currentProcess->priority++;
                enqueueToPriority(host.currentProcess);
                host.currentProcess = NULL;
            }
            else if(host.currentProcess->priority >= 3)
            {
                host.currentProcess->timeSpliceLeft = P3_TIMEQUANTUM;
                host.currentProcess->priority = 1;
                enqueueToPriority(host.currentProcess);
                host.currentProcess = NULL;
            }
        }
    }


    // Process termination/transcending of queues goes here
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
    if(queuePtr->process == NULL)
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
        if(currentProcess == NULL){
            printf("BROKEN!\n");
            broken = 1;
            break;
        }

        // If the process has "arrived", attempt to allocate resources to it
        if(time >= currentProcess->arrivalTime){
            //printf("HELLO from PID: %d!\n", currentProcess->pid);
            // Attempt to allocate resources to the process.
            int allocResult = allocateResources(currentProcess);

            // Resources were allocated successfully
            if(allocResult)
            {
                // Send the process to join the appropriate priority queue
                printf("Successful add!\n");
                enqueueToPriority(currentProcess);
                if(currentProcess->priority == 0) {
                    rtQUpdated = 1;
                    printf("RT: %d\n", rtQUpdated);
                }
                printf("\n\n");
            }
            // Resources could not be allocated for it.
            else
            {
                // Throw the process back on the dispatcher queue
                printf("Not enough resources!\n");
                enqueueProcess(newDispatcher, currentProcess);
            }
        }
        // The time has not come for this process, add it to the new dispatcher.
        else{
            enqueueProcess(newDispatcher, currentProcess);
        }
        i++;
    }
    // If the queue wasn't broken,
    if(!broken){
        if(dispatcher == NULL){
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
    if(p == 0)
    {
        if(!realTimeQueue)
            realTimeQueue = initializeQueue();
        enqueueProcess(realTimeQueue, process);
    }
    else if(p == 1)
    {
        if(!p1Queue)
            p1Queue = initializeQueue();
        enqueueProcess(p1Queue, process);
    }
    else if(p == 2)
    {
        if(!p2Queue)
            p2Queue = initializeQueue();
        enqueueProcess(p2Queue, process);
    }
    else
    {
        if(!p3Queue)
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
