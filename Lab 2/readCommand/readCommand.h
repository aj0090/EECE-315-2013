#include <stdio.h> // Used for standard in/out
#include <stdlib.h> // Functions for memory, like malloc, free, etc.
#include <unistd.h>
#include <string.h> // strsep(), etc.



#define MAX_ARGS 64
#define MAX_ARG_LENGTH 16
#define MAX_LINE_LENGTH 80
#define WHITESPACE " \n"
#define SPACE " "



struct command_t{
	char *name;
	int argc;
	char *argv[MAX_ARGS];
};



int runsInBackground(struct command_t *);
int readCommand(struct command_t *);
int parseCommand(char *, struct command_t *);
void printCommand(struct command_t *);
