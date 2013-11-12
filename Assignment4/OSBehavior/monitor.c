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
        printf("Version 1\n");
        printf("=========\n");
        numProc = readCPUInfo();
        printf("\n");
        readKernelVersion();
        printf("\n");
        readUptime("uptime", -1);
        printf("\n");
    }

    // v2:
    // time in: user mode, system (kernel?) mode, idle (/proc/uptime value 2)
    // number of disk io
    // number of context switches
    // time system was booted (calculate?)
    // number of processes created since boot
    if (version >= 2)
    {
        printf("Version 2\n");
        printf("=========\n");
        readStat(numProc);
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


// Prints "The amount of time the processor has spent in user  mode, system mode, and the
//         amount of time the system was idle"
// Param: numProc is the number of processors
// Return: none
void readStat(int numProc)
{
    char dest[80];
    char dest2[80];
    char line[500];
    float userTime, systemTime, idleTime;
    int i;
    char *token;

    const int USER_HZ = sysconf(_SC_CLK_TCK);

    FILE *fp;
    fp = fopen("/proc/stat", "r");
    if (!fp) exit(0);

    fgets(line, sizeof(line), fp);

    token = strtok(line, " ");
    i = 0;
    while (token != NULL)
    {
        if (i == 1)
            userTime = atof(token) / USER_HZ;
        else if (i == 3)
            systemTime = atof(token) / USER_HZ;
        else if (i == 4)
            idleTime = atof(token) / USER_HZ;
        token = strtok(NULL, " ");
        i++;
    }

    reprTime(userTime, dest);
    reprTime(userTime / numProc, dest2);
    printf("(System time, Processor time) in user mode: (%s, %s)\n", dest2, dest);
    reprTime(systemTime, dest);
    reprTime(systemTime / numProc, dest2);
    printf("(System time, Processor time) in system mode: (%s, %s)\n", dest2, dest);
    reprTime(idleTime, dest);
    reprTime(idleTime / numProc, dest2);
    printf("(System time, Processor time) spent idle: (%s, %s)\n", dest2, dest);

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
