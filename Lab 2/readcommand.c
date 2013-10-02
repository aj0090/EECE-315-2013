/*	readcommand.c
*	This test file shows the usage of the modules readCommand(), parseCommand,
*	and printCommand()
*	By Alan Larson
*/



#include <stdio.h> // Used for standard in/out
#include <stdlib.h> // Functions for memory, like malloc, free, etc.
#include <unistd.h>
#include <string.h> // strsep(), etc.

#define MAX_ARGS 64
#define MAX_ARG_LENGTH 16
#define MAX_LINE_LENGTH 80
#define WHITESPACE " \n"



struct command_t{
	char *name;
	int argc;
	char *argv[MAX_ARGS];
};


int readCommand(struct command_t *);
int parseCommand(char *, struct command_t *);
void printCommand(struct command_t *);git


int main(int argc, char *argv[])
{	
	struct command_t command;

	if(readCommand(&command))
		printCommand(&command);

	return 0;
}

// REQUIRES: A pointer to a command_t struct
// MODIFIES: current_command
// EFFECTS: Reads in user input through stdin and calls parseCommand to 
// set the struct members
int readCommand(struct command_t *current_command)
{
	// Create an input buffer and allocate it memory
	char *input_buffer;
	input_buffer = (char *)malloc(MAX_LINE_LENGTH);
	
	// Read in an input string from stdin and then parse the command into the
	// passed argument 'current_command'
	printf("Please enter the requested command and arguments.\n");
	if(parseCommand(fgets(input_buffer, MAX_LINE_LENGTH, stdin), current_command) != -1)
	{
		// Return 1 to signify successful read
		return 1;
	}

	// Return -1 to signify unsuccessful read
	return -1;

}

// REQUIRES: A non-null input string, and a non-null command_t struct
// MODIFIES: current_command
// EFFECTS: Parses strings seperated by the characters defined by WHITESPACE within  
// input_string into new blocks of memory and initializes current_commands argv, argc,
// and name members according to those values
int parseCommand(char *input_string, struct command_t *current_command)
{
	// Print to verify the input
	printf("Your command:\n%s", input_string);


	// Start the argument count 'argc' at 0 and allocate memory to the first array member
	int argc = 0;
	current_command->argv[argc] = (char *)malloc(MAX_ARG_LENGTH);

	// While strings are extracted from 'input_string' and set incrementally inside 
	// the struct 'current_command', allocate memory for the next command
	while((current_command->argv[argc] = strsep(&input_string, WHITESPACE)) != NULL){
		printf("%s\n", current_command->argv[argc]);
		current_command->argv[++argc] = (char*)malloc(MAX_ARG_LENGTH);
	}

	// Set the last argv member to be 1 and assign a decremented copy of argc to 
	// 'current_command'
	current_command->argv[argc] = NULL;
	current_command->argc = argc - 1;

	// Allocate memory to the 'name' member of the struct 'current_command'
	// and then copy the first argument's string to it
	current_command->name = malloc(sizeof(current_command->argv[0]));
	strcpy(current_command->name, current_command->argv[0]);

	return 1;
}

// REQUIRES: Non-null command_t struct with non-null pointers
// MODIFIES: NONE
// EFFECTS: Prints out information about current_command in a slightly
// formatted manner
void printCommand(struct command_t *current_command)
{
	int i;

	printf("Command Name: %s\n", current_command->name);
	printf("No. Arguments: %d\n", current_command->argc);

	for(i = 0; i < current_command->argc; i++)
	{
		printf("\t%d: %s\n", i, current_command->argv[i]);
	}

}
