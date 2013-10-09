#ifndef READCOMMAND_H
#define READCOMMAND_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// Constants
#define MAX_ARGS 64
#define MAX_PROG_LENGTH 16
#define MAX_ARG_LENGTH 128
#define MAX_LINE_LENGTH 1024
#define WHITESPACE  " \t\n"
#define SPACE " "

// Data Structures
struct command_t
{
    char *name;
    int argc;
    char *argv[MAX_ARGS];
    int runsInBackground;
    char *redirectFileName;
};

// Function Prototypes
const int runsInBackground(struct command_t *);

const int redirectsOutput(struct command_t *);
const char *redirectFileName(struct command_t *);

int checkAndSetRunsInBackground(struct command_t *);
int checkAndSetFileRedirection(struct command_t *);

int deleteLastArgument(struct command_t *);

int readCommand(struct command_t *);
int parseCommand(char *, struct command_t *);
void printCommand(struct command_t *);

#endif
