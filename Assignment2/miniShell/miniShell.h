#ifndef MINISHELL_H
#define MINISHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "readCommand/readCommand.h"
#include "lookupPath/lookupPath.h"
#include "externalCommands/externalCommands.h"


#define MAX_LINE_LEN    80
#define MAX_ARGS    64
#define MAX_ARG_LEN 16
#define MAX_PATHS   64

#define TRUE 1
#define FALSE 0

// Colors from: http://stackoverflow.com/questions/3219393/
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_CYAN    "\x1b[36m"


//Data Structures
/*
 *struct command_t{
 *    char *name;
 *    int argc;
 *    char *argv[MAX_ARGS];
 *    int runsInBackground;
 *    char *redirectFileName;
 *};
 */

//Function Prototypes
void printPrompt();
int parsePath(char **);

#endif
