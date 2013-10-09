//Made by Byron Duenas

#include "externalCommands.h"

//Remove/comment out main in final product
int main(int argc, char *argv[])
{
    if (argc != 3){
	fprintf(stderr,"Usage: test (folder you want to cd into) (folder you want to create)\n");
	return -1;
    }

    //Test if external command can execute
    struct command_t extCommand;

    extCommand.name = "cd";
    extCommand.argv[1] = "./";

    printf("isExternalCommand: %d\n", isExternalCommand(extCommand));

    extCommand.name = "pwd";

    printf("isExternalCommand: %d\n", isExternalCommand(extCommand));

    extCommand.name = "echo";
    extCommand.argv[1] = "Hello World";

    printf("isExternalCommand: %d\n", isExternalCommand(extCommand));

    extCommand.name = "mkdir";
    extCommand.argv[1] = "temp";

    printf("isExternalCommand: %d\n", isExternalCommand(extCommand));


    //Testing echo
    echoString("$PATH");

    //Testing printWorkingDirectory
    printWorkingDirectory();

    //Prompt should be set up properly
    printPrompt();

    //Test changing directory
    changeDirectory(argv[1]);

    //Testing printWorkingDirectory
    printWorkingDirectory();

    //Check that prompt updated its directory portion
    printPrompt();

    //To check if mkdir makes a directory in the new working directory
    int ret;
    char *cmd[] = { "mkdir", argv[2], (char *)0 };

    ret = execv ("/bin/mkdir", cmd);

    return 0;
}

// REQUIRES: NONE
// MODIFIES: NONE
// EFFECTS: Returns 0 if not an external command, returns 1 and executes command if otherwise
int isExternalCommand(struct command_t current_command) {
   if(strncmp(current_command.name, "pwd", 3) == 0) {
	printWorkingDirectory();
	return 1;
   }
   else if(strncmp(current_command.name, "cd", 2) == 0) {
	changeDirectory(current_command.argv[1]);
	return 1;
   }	
   else if(strncmp(current_command.name, "echo", 4) == 0) {
	echoString(current_command.argv[1]);
	return 1;
   }
   else
	return 0;	
}

// REQUIRES: NONE
// MODIFIES: The current working directory of the mini shell
// EFFECTS: The current working directory is changed via "cd dir"
void changeDirectory(char * dir) {
    char commandLine[COMMAND_LINE_MAX];

    //Generate string of form "cd dir; pwd"
    sprintf(commandLine, "cd %s; pwd", dir);

    //Call the generated command line string and redirect output of "pwd" to temp
    FILE *temp = popen(commandLine, "r");

    char newDir[DIR_NAME_MAX];

    //Store the redirected output of "pwd" to string newDir
    while (fgets(newDir, sizeof(newDir), temp) != 0)

    pclose(temp);

    int i;

    //Strip the new line at the end of the "pwd" output
    for(i = 0; i < sizeof(newDir); i++)
	if(newDir[i] == '\0')
	    newDir[i-1]='\0';

    //Actually change to newDir and check if it's valid
    if(chdir(newDir) < 0)
	printf("Change directory unsuccessful.\n");
    else
	printf("Change directory successful.\n");
}

// REQUIRES: NONE
// MODIFIES: NONE
// EFFECTS: Prints the current working directory
void printWorkingDirectory() {
    printf("%s\n", getcwd (NULL, 0));
}

// REQUIRES: NONE
// MODIFIES: NONE
// EFFECTS: Calls the echo command with the given str parameter
void echoString(char * str) {
    char echo[COMMAND_LINE_MAX];

    //Generate string of form "echo str"
    sprintf(echo, "echo %s", str);
    system(echo);
}

// REQUIRES: NONE
// MODIFIES: NONE
// EFFECTS: Prints out the prompt string after the execution of any command
void printPrompt() {
    char promptString[PROMPT_MAX];

    char hostname[HOST_NAME_MAX];

    gethostname(hostname, sizeof(hostname));

    //Set up promptString to be "user@machine:[currentWorkingDirectory]"
    sprintf(promptString,"%s@%s:[%s]", getenv("USER"), hostname, getcwd (NULL, 0));

    printf("%s", promptString);
    fflush(stdout);
}
