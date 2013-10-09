#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "miniShell.h"
#include "readCommand/readCommand.h"
#include "lookupPath/lookupPath.h"
#include "externalCommands/externalCommands.h"


// REQUIRES:
// MODIFIES:
// EFFECTS:
int main(int argc, char *argv[]) {
    int pid;
    int status;
    struct command_t command;
    char *path;
    int i;

    // Check that the program is run properly
    if (argc != 1) {
        fprintf(stderr, "Usage: miniShell (no arguments)\n");
        exit(0);
    }

    // Maintains the interactive shell
    while (TRUE) {
        printPrompt();

    /*cd/pwd will call printPrompt()*/

        // Read the command line and parse it
        readCommand(&command);

        if (!(command.name == NULL) && !isExternalCommand(&command)) {

            // Exit the interactive shell if command is 'exit' or 'quit'
            if (strncmp(command.name, "exit", 4) == 0 || strncmp(command.name, "quit", 4) == 0) {
                printf("Shell exiting...\n");
                exit(0);
            }

            // Create a child process to execute the command
            if ((pid = fork()) == 0) {
                /*
                 *if (redirectsOutput(&command)) {
                 *    printf("Child will need to redirect\n");
                 *}
                 */

                printf("I am the child\n");
                for (i = 0; i < command.argc; i++) {
                    printf("argument[%i]: %s\n", i, command.argv[i]);
                }
                printf("Path: %s\n", path);

                path = lookupPath(command.name);
                execv(path, command.argv);

                // Print execv errors
                printf("Error with execv %s\n", strerror(errno));
                status = 1;
                exit(0);
            } else {
                printf("I am the parent\n");
                if (!runsInBackground(&command)) {
                    printf("Parent will wait for child\n");
                    // Wait for children to terminate
                    wait(&status);
                }
            }

            // Free heap objects
            //free(path);
            command.name = NULL;
            // do we need to free command?

            printf("Post-fork\n");
        }
    }

    printf("\n\nShell terminated\n");
    return 0;
}
