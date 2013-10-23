#include "miniShell.h"


void sigintHandler()
{
    printf(ANSI_COLOR_YELLOW "\nShell exiting..." ANSI_COLOR_RESET "\n");
    exit(0);
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
    char *input_buffer;

    // Handle SIGINT (i.e., kill by Ctrl-C)
    signal(SIGINT, sigintHandler);

    // Check that the program is run properly
    if (argc != 1)
    {
        fprintf(stderr, "Usage: miniShell (no arguments)\n");
        exit(0);
    }

    // Maintains the interactive shell
    while (TRUE)
    {
        //Print the prompt
        printf(ANSI_COLOR_RESET);
        printPrompt();
        printf(ANSI_COLOR_CYAN);

        // Read the command line and parse it
        input_buffer = (char *)malloc(MAX_LINE_LENGTH);
        readResult = readCommand(input_buffer,  &command);

        if (readResult == -1)
        {
            printf(ANSI_COLOR_YELLOW "No executable command given." ANSI_COLOR_RESET "\n");
            free(input_buffer); //NEED THIS
            continue;
        }
        else if (readResult == -2)
        {
            printf(ANSI_COLOR_YELLOW "Command name too long." ANSI_COLOR_RESET "\n");
            free(input_buffer); //NEED THIS
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

            // If lookupPath returns NOT_FOUND, inform the user
            path = lookupPath(command.name);

            if (strncmp(path, NOT_FOUND, 12) == 0)
            {
                printf(ANSI_COLOR_RED"ERROR: We couldn't find `%s` in the PATH. :("ANSI_COLOR_RESET"\n", command.name);
                free(input_buffer);
                free(path);
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
            free(path);

        }
        free(input_buffer);
    }
    free(input_buffer);

    printf(ANSI_COLOR_RESET);
    exit(0);
}
