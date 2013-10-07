#define MAX_LINE_LEN    80
#define MAX_ARGS    64
#define MAX_ARG_LEN 16
#define MAX_PATHS   64
#define MAX_PATH_LEN    96

#define WHITESPACE  " .,\t\n"

#define TRUE 1
#define FALSE 0

#ifndef MINISHELL_H
#define MINISHELL_H
//Data Structures
struct command_t {
    char *name;
    int argc;
    char *argv[MAX_ARGS];
};
#endif

//Function Prototypes
void printPrompt();
void readCommand(char *);
int parseCommand(char *, struct command_t *);
int parsePath(char **);
char *lookupPath(char **, char **);
void printCommand(struct command_t *);
//runInBackground();
