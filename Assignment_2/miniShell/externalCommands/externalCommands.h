//Made by Byron Duenas

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define HOST_NAME_MAX 16
#define PROMPT_MAX 256
#define DIR_NAME_MAX 256
#define NEW_LINE "\n"
#define COMMAND_LINE_MAX 256

void changeDirectory( char * dir);
void printWorkingDirectory();
void echoString( char * str);
void printPrompt();
