//Made by Byron Duenas
#ifndef MINISHELL_H
#define MINISHELL_H


#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "../readCommand/readCommand.h"

#define HOST_NAME_MAX 16
#define PROMPT_MAX 256
#define DIR_NAME_MAX 256
#define NEW_LINE "\n"
#define COMMAND_LINE_MAX 256

#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RESET   "\x1b[0m"


int isExternalCommand(struct command_t *current_command);
void changeDirectory( char *dir);
void printWorkingDirectory();
void echoString( char *str);
void printPrompt();


#endif
