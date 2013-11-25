#include "hostd.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        // Print directions if the file is not supplied and exit
        printf("You must supply a file to the program:\n"
                "$ hostd filename.txt\n");
        exit(0);
    }

    /*initialize the system*/
    /*set time to 0*/
    /*set resource counters*/
    /*set memory limit*/
    /*init all queues*/
    cpuTime = 0;
    dispatcher = initializeQueue();
    printf("Initialized!\n");

    char *filename = argv[1];
    readFile(filename, dispatcher);

    // DEBUG
    printQueue(dispatcher);

    return 0;
}

// Creates processes from the file given and adds them to the dispatcher queue
// @par: pointer        filename
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
        printf("The file %s was found.\n", filename);
    }

    char fileBuffer[FILE_BUFFER];
    fscanf(fp, "%[^\n]", fileBuffer);

    /*scan for process*/
    /*init process*/
    /*add process info*/
    /*put process in dispatch queue*/
    /*scan for process*/

    //DEBUG
    replaceMe *process = malloc((sizeof(replaceMe)));
    *process = 8;
    enqueueProcess(dispatcher, process);

    printf("fileBuffer: %s\n", fileBuffer);
}

