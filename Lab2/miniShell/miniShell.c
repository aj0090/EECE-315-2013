#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "minishell.h"

int main(int argc, char *argv[]) {
    int i;
    int pid, numChildren;
    int status;
    FILE *fid;
    char cmdLine[MAX_LINE_LEN];
    struct command_t command;

    /*Read the command line parameters*/
    if (argc != 1) {
        fprintf(stderr, "Usage: miniShell (no arguments)\n");
        exit(0);
    }

    /*Shell initialization, get directory paths from PATH*/
    /*parsePath(pathv);*/

    while (TRUE) {
        /*printPrompt();*/

        /*Read the command line and parse it*/
        /*readCommand(commandLine);*/
        /*parseCommand(commandLine, &command);*/

        /*Get the full pathname for the file*/
        /*
         *command.name = lookupPath(command.argv, pathv);
         *if (command.name == NULL) {
         *    Report error
         *    continue;
         *}
         */

        /*Create a child process to execute the command*/
        /*
         *if ((pid = fork()) == 0) {
         *    execv(command.name, command.argv);
         *}
         */

        /*If "&", then parent continues to next command*/
        /*numChildren++;*/

        /*Wait for all children to terminate*/
        /*
         *for (i = 0; i < numChildren; i++) {
         *    wait(&status);
         *    //should free dynamic storage in command data structure
         *}
         */

        /*Exit the shell when user inputs a quit command*/
    }

    printf("\n\nlaunch: Terminating successfully\n");
    return 0;
}
