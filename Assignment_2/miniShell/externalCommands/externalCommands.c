//Made by Byron Duenas

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define HOST_NAME_MAX 16
#define PROMPT_MAX 256

int changeDirectory( char * dir);
void printWorkingDirectory();
void echoString( char * str);
void printPrompt();

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		fprintf(stderr,"An integer parameter is required\n");
		return -1;
	}

    printPrompt();
    printWorkingDirectory();


    int changeSuccess = changeDirectory(argv[1]);

    printPrompt();
    printWorkingDirectory();

    printf("%d\n", changeSuccess);

    //To check if mkdir makes a directory in the new working directory
    int ret;
    char *cmd[] = { "mkdir", argv[2], (char *)0 };

    ret = execv ("/bin/mkdir", cmd);


    return 0;
}

int changeDirectory(char * dir) {
    char newDir[300];

    //Concatenate old working directory with desired directory
    sprintf(newDir, "cd %s", dir);

    //Returns < 0 if chdir fails
    return system(newDir);
}

void printWorkingDirectory() {
    printf("%s\n", getenv("PWD"));
}

void echoString(char * str) {
    printf("%s\n", str);
}

void printPrompt() {
    char promptString[PROMPT_MAX];

    char hostname[HOST_NAME_MAX];

    //validHostname is negative if gethostname fails
    int validHostname = gethostname(hostname, sizeof(hostname));

    //Set up promptString to be "user@machine:[currentWorkingDirectory]"
    sprintf(promptString,"%s@%s:[%s]", getenv("USER"), (validHostname < 0) ? "" : hostname, getenv("PWD"));

    printf("%s", promptString);
}
