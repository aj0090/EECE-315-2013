#include "externalCommands.h"

// REQUIRES: NONE
// MODIFIES: NONE
// EFFECTS: Returns 0 if not an external command, returns 1 and executes command if otherwise
int isExternalCommand(struct command_t *current_command)
{
    int fd, stdoutCopy;

    // If redirection flag is set in command, redirect output
    if (redirectsOutput(current_command))
    {
        // Open filename given with read/write (create if doesn't exist), set read and write permissions for owner
        fd = open(redirectFileName(current_command), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
        stdoutCopy = dup(1);

        // Duplicates fd as STDOUT and closes STDOUT, effectively redirecting output
        dup2(fd, 1);
        close(fd);
    }

    if (strncmp(current_command->name, "pwd", 3) == 0)
    {
        printWorkingDirectory();
    }
    else if (strncmp(current_command->name, "cd", 2) == 0)
    {
        changeDirectory(current_command->argv[1]);
    }
    else if (strncmp(current_command->name, "echo", 4) == 0)
    {
        echoString(current_command->argv[1]);
    }
    else
    {
        dup2(stdoutCopy, 1);
        close(stdoutCopy);
        return 0;
    }

    dup2(stdoutCopy, 1);
    close(stdoutCopy);
    return 1;
}

// REQUIRES: NONE
// MODIFIES: The current working directory of the mini shell
// EFFECTS: The current working directory is changed via "cd dir"
void changeDirectory(char *dir)
{
    char commandLine[COMMAND_LINE_MAX];
    char home[] = "~";
    char *realDir;
    int i;

    // If no dir argument provided, go home (standard Bash behaviour)
    if (dir != NULL)
    {
        realDir = dir;
    }
    else
    {
        realDir = (char *) &home;
    }

    // Generate string of form "cd dir; pwd"
    sprintf(commandLine, "cd %s; pwd", realDir);

    // Call the generated command line string and redirect output of "pwd" to temp
    FILE *temp = popen(commandLine, "r");
    char newDir[DIR_NAME_MAX];

    // Store the redirected output of "pwd" to string newDir
    while (fgets(newDir, sizeof(newDir), temp) != 0);

    pclose(temp);

    // Strip the new line at the end of the "pwd" output
    for (i = 0; i < sizeof(newDir); i++) {
        if (newDir[i] == '\0') {
            newDir[i - 1] = '\0';
        }
    }

    // Actually change to newDir and check if it's valid
    if (chdir(newDir) < 0)
    {
        printf("Change directory unsuccessful.\n");
        printf("Error: %s\n", strerror(errno));
    }
    else
    {
        printf("Change directory successful.\n");
    }
}

// REQUIRES: NONE
// MODIFIES: NONE
// EFFECTS: Prints the current working directory
void printWorkingDirectory()
{
    printf("%s\n", getcwd (NULL, 0));
}

// REQUIRES: NONE
// MODIFIES: NONE
// EFFECTS: Calls the echo command with the given str parameter
void echoString(char *str)
{
    char echo[COMMAND_LINE_MAX];

    // Generate string of form "echo str"
    sprintf(echo, "echo %s", str);
    system(echo);
}

// REQUIRES: NONE
// MODIFIES: NONE
// EFFECTS: Prints out the prompt string after the execution of any command
void printPrompt()
{
    char cwd[PROMPT_MAX];
    char hostname[HOSTNAME_MAX];

    gethostname(hostname, sizeof(hostname));

    printf("EECE315_$HELL<>%s@%s:", getenv("USER"), hostname);
    sprintf(cwd, "%s", getcwd(NULL, 0));
    printf(ANSI_COLOR_GREEN "%s" ANSI_COLOR_RESET "\n$ ", cwd);

    fflush(stdout);
}
