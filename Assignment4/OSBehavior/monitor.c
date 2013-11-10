#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void readCPUInfo(void);
void readKernelVersion(void);
void readUptime(void);

int main(int argc, char *argv[]) {
    // v1:
    // processor type /proc/cpuinfo
    // kernel version /proc/sys/kernel/version
    // uptime /proc/uptime
    //
    // v2:
    // time in: user mode, system (kernel?) mode, idle (/proc/uptime value 2)
    // number of disk io
    // number of context switches
    // time system was booted (calculate?)
    // number of processes created since boot
    //
    // v3:
    // amount of memory configured /proc/meminfo
    // amount of memory available /proc/meminfo
    // list of load averages (default? averaged over last min)
    // monitor -l 2 60 => observe 60s, sampling every 2s to scan multiple values: fscanf(fp, "%s %s", string1, string2);

    readCPUInfo();
    readKernelVersion();
    readUptime();
    return 0;
}

// Prints the kernel version
// Param: none
// Return: none
void readKernelVersion(void) {
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
// Return: none
void readCPUInfo(void) {
    char line[500];
    size_t length = 0;
    ssize_t read;

    char *key, *value;
    key = malloc(10 * sizeof(char));
    value = malloc(1000 * sizeof(char));

    FILE *fp;
    fp = fopen("/proc/cpuinfo", "r");
    if (!fp) exit(0);
    while (fgets(line, sizeof(line), fp) != NULL) {
        strcpy(key, "n/a");
        strcpy(value, "n/a");
        if (sscanf(line, " %[^:]: %[^\n]", key, value) > 0) {
            if (strstr(key, "processor")) {
                printf("Processor %s is a ", value);
            }
            if (strstr(key, "model name")) {
                printf("%s\n", value);
            }
        }
    }

    if (key) free(key);
    if (value) free(value);
    fclose(fp);
}

// Prints the uptime of the system
// Param: none
// Return: none
void readUptime(void) {
    char uptime[10]; // enough digits for ~115 days
    double uptimeSec, uptimeMin, uptimeHr, uptimeDay = 0;

    FILE *fp;
    fp = fopen("/proc/uptime", "r");
    if (!fp) exit(0);
    fscanf(fp, "%s", uptime);

    uptimeSec = atof(uptime);
    uptimeMin = uptimeSec / 60;
    uptimeHr = uptimeMin / 60;
    uptimeDay = uptimeHr / 24;

    /*printf("Uptime: %.2f (seconds), %.2f (minutes), %.2f (hours), %.2f (days).\n", uptimeSec, uptimeMin, uptimeHr, uptimeDay);*/
    printf("Uptime: %.2f seconds\n", uptimeSec);
    fclose(fp);
}
