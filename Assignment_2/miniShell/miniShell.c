#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "minishell.h"

// REQUIRES:
// MODIFIES:
// EFFECTS:
int main(int argc, char *argv[]) {
    int pid;
    int status;
    char commandLine[MAX_LINE_LEN];
    struct command_t command;

    // Check that the program is run properly
    if (argc != 1) {
        fprintf(stderr, "Usage: miniShell (no arguments)\n");
        exit(0);
    }

    // Maintains the interactive miniShell
    /*while (TRUE) {*/
        /*printPrompt();*/

    /*cd/pwd will call printPrompt()*/

        /*Read the command line and parse it*/
        /*readCommand(commandLine);*/
        /*parseCommand(commandLine, &command);*/

        // TEMPORARY VARIABLE
        command.name = "exit";

        // Exit the interactive miniShell if command is 'exit' or 'quit'
        if (strncmp(command.name, "exit", 4) == 0 || strncmp(command.name, "quit", 4) == 0) {
            printf("exiting...\n");
            exit(0);
        }

        /*Get the full pathname for the file*/
        /*
         *command.name = lookupPath(command.argv, pathv);
         *if (command.name == NULL) {
         *    Report error
         *    continue;
         *}
         */

        /*Create a child process to execute the command*/
        if ((pid = fork()) == 0) {
            printf("I am the child\n");
            /*execv(command.name, command.argv);*/
            status = 1;
            exit(0);
        } else {
            /*if (!runsInBackground(&command)) {*/
                /*Wait for all children to terminate*/
                wait(&status);
            /*}*/
        }

        printf("I am the parent\n");
        //free dynamic storage in command data structure here
        printf("after fork\n");

        /*Exit the shell when user inputs the "quit" and "exit" command*/
    /*}*/

    printf("\n\nlaunch: Terminating successfully\n");
    return 0;
}
