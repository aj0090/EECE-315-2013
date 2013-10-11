#include "externalCommands.h"

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: test (folder you want to cd into) (folder you want to create)\n");
        return -1;
    }

    //Test if external command can execute
    struct command_t extCommand;

    extCommand.name = "cd";
    extCommand.argv[1] = "./";

    printf("isExternalCommand: %d\n", isExternalCommand(&extCommand));

    extCommand.name = "pwd";

    printf("isExternalCommand: %d\n", isExternalCommand(&extCommand));

    extCommand.name = "echo";
    extCommand.argv[1] = "Hello World";

    printf("isExternalCommand: %d\n", isExternalCommand(&extCommand));

    extCommand.name = "mkdir";
    extCommand.argv[1] = "temp";

    printf("isExternalCommand: %d\n", isExternalCommand(&extCommand));

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

    ret = execv("/bin/mkdir", cmd);

    return 0;
}
