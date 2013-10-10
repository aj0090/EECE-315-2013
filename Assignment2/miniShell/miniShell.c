#include "miniShell.h"


void free_command(struct command_t *command)
{
    int i;
    // Free heap objects
    free(command->name); //8 bytes
    for (i = 0; i < command->argc; i++)
    {
        free(command->argv[i]);
    }
    free(command->redirectFileName); //8 bytes
}


// REQUIRES: NONE
// MODIFIES: NONE
// EFFECTS: Runs the miniShell
int main(int argc, char *argv[])
{
    int pid;
    int status;
    char *path;
    int i, fd, readResult;
    struct command_t command;

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
                printf(ANSI_COLOR_YELLOW "Shell exiting..." ANSI_COLOR_RESET "\n");
                break;
            }

            // If lookupPath returns FILENOTFOUND, inform the user
            path = lookupPath(command.name);

            if (strncmp(path, "FILENOTFOUND", 12) == 0)
            {
                printf(ANSI_COLOR_RED"ERROR: We couldn't find `%s` in the PATH. :("ANSI_COLOR_RESET"\n", command.name);
                continue;
            }

            // Create a child process to execute the command
            if ((pid = fork()) == 0)
            {
                // If redirection flag is set in command, redirect output
                if (redirectsOutput(&command))
                {
                    // Open filename given with read/write (create if doesn't exist), set read and write permissions for owner
                    fd = open(redirectFileName(&command), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

                    // Duplicates fd as STDOUT and closes STDOUT, effectively redirecting output
                    dup2(fd, 1);
                    close(fd);
                }

                execv(path, command.argv);

                // Print execv errors
                printf(ANSI_COLOR_RED"ERROR: %s"ANSI_COLOR_RESET, strerror(errno));
                status = 1;
                exit(0);
            }
            else
            {
                if (!runsInBackground(&command))
                {
                    // Wait for children to terminate
                    wait(&status);
                }
            }

        }

        //free_command(&command);
    }
    //free_command(&command);

    exit(0);
}
