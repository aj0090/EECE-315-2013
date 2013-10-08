#include <stdio.h> // Used for standard in/out
#include <stdlib.h> // Functions for memory, like malloc, free, etc.
#include <unistd.h>
#include <string.h> // strsep(), etc.
#include "readCommand.h"


int main(int argc, char *argv[])
{	
	struct command_t command;

	if(readCommand(&command))
		printCommand(&command);
	
	if(runsInBackground(&command))
		printf("Runs in background.\n");
	else
		printf("Doesn't run in background.\n");
	
	if(redirectsOutput(&command)){
		printf("Redirects output to %s\n", redirectFileName(&command));
	}
	else{
		printf("Does not redirect.\n");
	}

	return 0;
}
