#include "readCommand.h"


// REQUIRES: A fully initialized command_t struct read in through readCommand
// MODIFIES: None
// EFFECTS: Returns the value of current_command->runsInBackground
const int runsInBackground(struct command_t *current_command)
{
	return current_command->runsInBackground;
}


// REQUIRES: A fully initialized struct built within readCommand
// MODIFIES: None
// EFFECTS: Returns if current_command->redirectFileName == NULL
const int redirectsOutput(struct command_t *current_command)
{
	return current_command->redirectFileName == NULL ? 0 : 1;
}

// REQUIRES: redirectsOutput(current_command) == 1
// MODIFIES: None
// EFFECTS: Returns the string of the filename the program will redirect
// its output to.
const char *redirectFileName(struct command_t *current_command)
{
	// Return the char *redirectFileName
	return current_command->redirectFileName;
}



// REQUIRES: A non-null fully initialized command_t struct
// MODIFIES: current_command->argc, current_command->argv, current_command->runsInBackground
// EFFECTS: If the last argument is an &, set current_command->runsInBackground to 1, and
// delete the last argument in current_command. Else, set current_command->runsInBackground to 0,
// and do not delete the last argument.
int checkAndSetRunsInBackground(struct command_t *current_command)
{
	if(current_command->argc < 2 || strcmp(current_command->argv[current_command->argc - 1], "&"))	{
		current_command->runsInBackground = 0;
	}
	// If last argument == "&", delete the last argument and set runsInBackground to 1
	else{
		deleteLastArgument(current_command);
		current_command->runsInBackground = 1;
	}

	return 1;
}



// REQUIRES: A non-null fully initialized struct that has had
// the function checkAndSetRunsInBackground() run
// MODIFIES: current_command->redirectFileName, current_command->argc,
// current_command->argv
// EFFECTS: Allocates memory for current_command->redirectFileName.
// If the current_command->argv[current_command->argc - 2] == ">", set redirectFileName
// to current_command->argv[current_command->argc - 1], and delete the last two arguments.
// Else, set it to NULL. Returns 1 on completion.
int checkAndSetRedirect(struct command_t *current_command)
{
	// Allocate memory for the file name
	current_command->redirectFileName = malloc(MAX_ARG_LENGTH);
	current_command->redirectFileName = NULL;

	// If the second last argument == ">", set the redirect Filename to the last argument,
	// and delete last two arguments
	if(current_command->argc >= 3)
		if(!strcmp(current_command->argv[current_command->argc - 2], ">")){
			current_command->redirectFileName = current_command->argv[current_command->argc - 1];
			deleteLastArgument(current_command);
			deleteLastArgument(current_command);
		}

	return 1;
}


// REQUIRES: current_command->argc >= 0, current_command->argv[argc] has valid memory
// and its content == NULL
// MODIFIES: current_command->argc, current_command->argv
// EFFECTS: Free's current_command->argv[current_command->argc], decrements current_command->argc,
// and then sets current_command->argv[current_command->argc] to NULL. Returns 1 on completion.
int deleteLastArgument(struct command_t *current_command)
{
	// Free the memory of the argv after the char* array, and decrement the count to reflect
	// the freeing of the memory
	free(current_command->argv[current_command->argc--]);

	// Make the new argv after the char* = NULL
	current_command->argv[current_command->argc] = NULL;
	return 1;
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
	fgets(input_buffer, MAX_LINE_LENGTH, stdin);
    // Checking for null input
    if(strcmp(input_buffer, "\n")){
        if(parseCommand(input_buffer, current_command) != -1)
        {
            // Return 1 to signify successful read
            checkAndSetRunsInBackground(current_command);
            checkAndSetRedirect(current_command);
            return 1;
        }
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
	// Start the argument count 'argc' at 0 and allocate memory to the first array member
	int argc = 0;
	current_command->argv[argc] = (char *)malloc(MAX_ARG_LENGTH);

	// While strings are extracted from 'input_string' and set incrementally inside
	// the struct 'current_command', allocate memory for the next command
	while((current_command->argv[argc] = strsep(&input_string, WHITESPACE)) != NULL){
		if(current_command->argv[argc][0] != 0)
			current_command->argv[++argc] = (char*)malloc(MAX_ARG_LENGTH);
		// RE-ENABLE THIS IF YOU WANT FUNCTION TO EXIT IF ANY NULL ARGS ARE GIVEN
		/*else
			return -1
			*/
	}
	printf("%d", current_command->argc);

	current_command->argv[++argc] = (char*)malloc(MAX_ARG_LENGTH);

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
