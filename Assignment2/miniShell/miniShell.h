#ifndef MINISHELL_H
#define MINISHELL_H

#define MAX_LINE_LEN    80
#define MAX_ARGS    64
#define MAX_ARG_LEN 16
#define MAX_PATHS   64
#define MAX_PATH_LEN    96

#define WHITESPACE  " .,\t\n"

#define TRUE 1
#define FALSE 0

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
