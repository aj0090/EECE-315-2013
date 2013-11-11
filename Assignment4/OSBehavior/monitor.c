#include "monitor.h"


// Displays information about the system
// Param: none|-l string<int> string<int>
// Return: 0
int main(int argc, char *argv[])
{
    int version = identify_args(argc, argv);
    int numProc;

    // v1:
    if (version >= 1)
    {
        numProc = readCPUInfo();
        readKernelVersion();
        readUptime("uptime", -1);
    }

    // v2:
    // time in: user mode, system (kernel?) mode, idle (/proc/uptime value 2)
    // number of disk io
    // number of context switches
    // time system was booted (calculate?)
    // number of processes created since boot
    if (version >= 2)
    {
        printf("\n");
        readUptime("idleTime", numProc);
    }

    // v3:
    // amount of memory configured /proc/meminfo
    // amount of memory available /proc/meminfo
    // list of load averages (default? averaged over last min)
    // monitor -l 2 60 => observe 60s, sampling every 2s to scan multiple values: fscanf(fp, "%s %s", string1, string2);

    return 0;
}


// Prints the kernel version
// Param: none
// Return: none
void readKernelVersion(void)
{
    char version[100];

    FILE *fp;
    fp = fopen("/proc/sys/kernel/version", "r");
    if (!fp) exit(0);
    fscanf(fp, "%[^\n]", version);

    printf("Kernel version: %s\n", version);
    fclose(fp);
}


// Prints the processor type
// Param: none
// Return: (int) Number of processors
int readCPUInfo(void)
{
    int numProc;
    char line[500];
    size_t length = 0;
    ssize_t read;

    char *key, *value;
    key = malloc(10 * sizeof(char));
    value = malloc(1000 * sizeof(char));

    FILE *fp;
    fp = fopen("/proc/cpuinfo", "r");
    if (!fp) exit(0);
    while (fgets(line, sizeof(line), fp) != NULL)
    {
        strcpy(key, "n/a");
        strcpy(value, "n/a");
        if (sscanf(line, " %[^:]: %[^\n]", key, value) > 0)
        {
            if (strstr(key, "processor"))
            {
                printf("Processor %s is a ", value);
                numProc = atoi(value);
            }
            if (strstr(key, "model name"))
            {
                printf("%s\n", value);
            }
        }
    }

    if (key) free(key);
    if (value) free(value);
    fclose(fp);

    return numProc + 1;
}


// Prints the uptime of the system, or idleTime if requested
// Param: val is string representing what value to print, numProc number of processors
// Return: 0 is success, -1 is invalid val
int readUptime(char *val, int numProc)
{
    char dest[80];
    char uptime[10], idleTime[10]; // enough digits for ~115 days
    double uptimeSec, idleTimeSec;

    FILE *fp;
    fp = fopen("/proc/uptime", "r");
    if (!fp) exit(0);
    fscanf(fp, "%s %s", uptime, idleTime);
    fclose(fp);

    uptimeSec = atof(uptime);
    idleTimeSec = atof(idleTime);

    if (!strcmp("uptime", val))
    {
        reprTime(uptimeSec, dest);
        printf("Uptime: %s\n", dest);
        return 0;
    }
    else if (!strcmp("idleTime", val))
    {
        // Divide by the number of processor cores to get the actual idle time
        reprTime(idleTimeSec / numProc, dest);
        printf("System idle time: %s\n", dest);
        return 0;
    }
    else
        return -1;
}
