#include "utils.h"


// Validates passed arguments
// Param: argc is number of arguments, argv is array of arguments
// Return: (int) which version of program to run
int identify_args(int argc, char *argv[])
{
    int i;
    const char usage[] = "Usage: ./monitor -v[1..3] [sleepTime] [runTime]\n"
                         "e.g.,  ./monitor -v2\n"
                         "e.g.,  ./monitor -v3 0.5 5";

    switch (argc)
    {
    case 2:
        if (!strcmp("-v1", argv[1]))
            return 1;
        else if (!strcmp("-v2", argv[1]))
            return 2;
        else if (!strcmp("-v3", argv[1]))
        {
            printf("-v3 needs two additional arguments.\n\n%s\n", usage);
            exit(1);
        }
        else
        {
            printf("Invalid version argument.\n\n%s\n", usage);
            exit(1);
        }
    case 4:
        if (strcmp("-v3", argv[1]))
        {
            printf("Invalid version argument.\n\n%s\n", usage);
            exit(1);
        }
        else
            for (i = 2; i <= 3; i++)
                if (!atof(argv[i]))
                {
                    printf("Invalid parameters (must be numbers).\n\n%s\n", usage);
                    exit(1);
                }
        return 3;
    default:
        printf("Invalid number of arguments.\n\n%s\n", usage);
        exit(1);
    }
}



// Takes seconds and sprints a <day>d <hours>h <minutes>m <seconds>s repr of it dest
// Param: seconds is seconds to convert, dest is destination string
// Return: none
void reprTime(float seconds, char *dest)
{
    // Convert seconds to floats for each measure
    float minutes = seconds / 60;
    float hours = minutes / 60;
    float days = hours / 24;

    // Subtract off remainders to get actual values
    hours = (days - (int) days) * 24;
    minutes = (hours - (int) hours) * 60;
    float secs = (minutes - (int) minutes) * 60;

    // e.g., 8d 5h 3m 10s
    sprintf(dest, "%dd %dh %dm %ds", (int) days, (int) hours, (int) minutes, (int) secs);
}
