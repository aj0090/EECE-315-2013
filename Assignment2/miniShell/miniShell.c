#include "miniShell.h"


// REQUIRES:
// MODIFIES:
// EFFECTS:
int main(int argc, char *argv[])
{
    int pid;
    int status;
    struct command_t command;
    char *path;
    int i, fd, readResult;

    // Check that the program is run properly
    if (argc != 1)
    {
        fprintf(stderr, "Usage: miniShell (no arguments)\n");
        exit(0);
    }

    // Maintains the interactive shell
    while (TRUE)
    {
        printPrompt();

        // Read the command line and parse it
        readResult = readCommand(&command);
        if (readResult == -1)
        {
            printf(ANSI_COLOR_YELLOW "No executable command given." ANSI_COLOR_RESET "\n");
            continue;
        }
        else if (readResult == -2)
        {
            printf(ANSI_COLOR_YELLOW "Command name too long." ANSI_COLOR_RESET "\n");
            continue;
        }

        // Ensure command is not handled by externalCommand
        if (!isExternalCommand(&command))
        {
            // Exit the interactive shell if command is 'exit' or 'quit'
            if (strncmp(command.name, "exit", 4) == 0 || strncmp(command.name, "quit", 4) == 0)
            {
                printf(ANSI_COLOR_YELLOW "Shell exiting . . ." ANSI_COLOR_RESET "\n");
                exit(0);
            }

            // If lookupPath returns FILENOTFOUND, inform the user
            //printf("Command name: %s\n", command.name); //DEBUGGING
            path = lookupPath(command.name);
            //printf("Path: %s\n", path); //DEBUGGING
            if (strncmp(path, "FILENOTFOUND", 12) == 0)
            {
                printf(ANSI_COLOR_RED"ERROR: We couldn't find `%s` in the"
                       " PATH. :("ANSI_COLOR_RESET"\n", command.name);
                continue;
            }

            // Create a child process to execute the command
            if ((pid = fork()) == 0)
            {

                // If redirection flag is set in command, redirect output
                if (redirectsOutput(&command))
                {
                    // Open filename given with read/write (create if doesn't exist),
                    // set read and write permissions for owner
                    fd = open(redirectFileName(&command), O_RDWR | O_CREAT,
                              S_IRUSR | S_IWUSR);
                    dup2(fd, 1);  // Duplicates fd as STDOUT and closes STDOUT,
                    // effectively redirecting output
                    close(fd);
                }

                // DEBUGGING STATEMENTS
                /*printf("I am the child\n");
                for (i = 0; i < command.argc; i++)
                {
                    printf("argument[%i]: %s\n", i, command.argv[i]);
                }
                */

                execv(path, command.argv);

                // Print execv errors
                // printf("Error with execv %s\n", strerror(errno)); // DEBUGGING
                printf(ANSI_COLOR_RED"ERROR: %s"ANSI_COLOR_RESET"\n", strerror(errno));
                status = 1;
                exit(0);
            }
            else
            {
                // printf("I am the parent\n"); // DEBUGGING
                if (!runsInBackground(&command))
                {
                    // printf("Parent will wait for child\n"); // DEBUGGING
                    // Wait for children to terminate
                    wait(&status);
                }
            }

            // Free heap objects
            //free(path);
            command.name = NULL;
            // XXX do we need to free command?

            // printf("Post-fork\n"); // DEBUGGING
        }
    }

    printf("\n\nShell terminated\n");
    return 0;
}
