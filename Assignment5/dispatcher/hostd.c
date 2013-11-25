#include <stdio.h>
#include <stdlib.h>
/*#include <unistd.h>*/
/*#include <signal.h>*/
/*#include <sys/types.h>*/
#include "processes/processes.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        // Print directions if the file is not supplied and exit
        printf("You must supply a file to the program:\n"
                "$ hostd filename.txt\n");
        exit(0);
    }
    char *filename = argv[1];

    return 0;
}

void readFile(char *filename) {
    FILE *fp;
    fp = fopen(filename, "r");
    if (!fp) {
        // Print error if the file was not found and exit
        printf("The file %s was not found.", filename);
        exit(0);
    }
}
