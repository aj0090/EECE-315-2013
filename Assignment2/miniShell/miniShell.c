#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "minishell.h"
#include "readCommand/readCommand.h"
#include "lookupPath/lookupPath.h"

// REQUIRES:
// MODIFIES:
// EFFECTS:
int main(int argc, char *argv[]) {
    int pid;
    int status;
    /*char path[MAX_LINE_LEN];*/
    struct command_t command;

    // Check that the program is run properly
    if (argc != 1) {
        fprintf(stderr, "Usage: miniShell (no arguments)\n");
        exit(0);
    }

    // Maintains the interactive miniShell
    while (TRUE) {
        /*printPrompt();*/

    /*cd/pwd will call printPrompt()*/

        // Read the command line and parse it
        readCommand(&command);

        // Exit the interactive shell if command is 'exit' or 'quit'
        if (strncmp(command.name, "exit", 4) == 0 || strncmp(command.name, "quit", 4) == 0) {
            printf("Shell exiting...\n");
            exit(0);
        }

        /*
         *if (command.name == NULL) {
         *    [>Report error<]
         *    continue;
         *}
         */

        /*Create a child process to execute the command*/
        if ((pid = fork()) == 0) {
            /*
             *if (redirectsOutput(&command)) {
             *    printf("Child will need to redirect\n");
             *}
             */
            printf("I am the child\n");
            /*printf("Path for '%s' is '%s'\n", command.name, lookupPath(command.name));*/
            /*printf("arguments: %s", command.argv[1]);*/
            execv(lookupPath(command.name), command.argv);
            /*execv(command.name, command.argv);*/
            status = 1;
            exit(0);
        } else {
            printf("I am the parent\n");
            if (!runsInBackground(&command)) {
                printf("Parent will wait for child\n");
                /*Wait for all children to terminate*/
                wait(&status);
            }
        }

        //free dynamic storage in command data structure here
        printf("after fork\n");
    }

    printf("\n\nShell terminated\n");
    return 0;
}
