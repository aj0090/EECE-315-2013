#include "../hostd.h"

// Creates processes from the file given and adds them to the dispatcher queue
// @par: char pointer   filename
// @par: Queue pointer  dispatcher queue
// @ret: none
void readFile(char *filename, Queue *dispatcher) {
    FILE *fp;
    fp = fopen(filename, "r");

    if (!fp) {
        // Print error if the file was not found and exit
        printf("The file %s was not found, exiting.\n", filename);
        exit(0);
    } else {
        if (VERBOSE) printf("The file %s was found.\n", filename);
    }

    // Character string for line contents
    char *line = malloc(FILE_BUFFER * sizeof(char));

    // Static int to keep track of PID's, starting at id = 1, increments between invocations for readFile()
    static int pid = 1;
    runtime = 0;

    // While process data exists and total number of processes < maximum amount
    while (fgets(line, FILE_BUFFER, fp) != NULL && pid <= MAX_PROCESSES) {
        line = strtok(line, "\n");

        // Create a new process
        PCB *process = newPCB();

        // Parse process data
        int *args = stringToInts(line);
        // args[0] - arrival time
        // args[1] - priority
        // args[2] - processor time
        // args[3] - Mbytes
        // args[4] - # printers
        // args[5] - # scanners
        // args[6] - # modems
        // args[7] - # cds

        // Add the pid and arguments to the PCB
        process->pid = pid++;
        process->priority = args[1];
        process->arrivalTime = args[0];
        process->remainingTime = args[2];
        process->state = STARTED;

        // Max of (biggest arrival time + runtime) or sum of all runtimes
        runtime += process->remainingTime;
        if ((process->arrivalTime + process->remainingTime) > runtime) {
            runtime = process->arrivalTime + process->remainingTime;
        }

        // Allocate required resources for the process
        process->IO->memSpaceNeeded = args[3];
        process->IO->printersNeeded = args[4];
        process->IO->scannersNeeded = args[5];
        process->IO->modemsNeeded = args[6];
        process->IO->disksNeeded = args[7];

        if (VERBOSE) printPCBInfo(process);

        free(args);

        // Add process to dispatcher queue
        enqueueProcess(dispatcher, process);

        // TODO: do we need to free anything else? does freeing process clear queue?
        /*free(process);*/
    }

    if (VERBOSEQ) printQueue(dispatcher);

    // Free our memory
    free(line);
    fclose(fp);
}

// Converts the string to int tokens
// @par: char pointer   string
// @ret: int pointer    array of int tokens
int *stringToInts(char *string) {
    // Allocate new array with size 8 since file input includes 8 ints
    int *numbers = malloc(sizeof(int) * 8);
    char *pt;
    pt = strtok(string, ", ");

    int count = 0;
    while (pt != NULL && count < 8) {
        numbers[count++] = atoi(pt);
        pt = strtok(NULL, ", ");
    }

    return numbers;
}
