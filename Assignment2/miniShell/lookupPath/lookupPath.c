#include "lookupPath.h"

// Reference: http://www.cplusplus.com/reference/cstring/strtok/
char *lookupPath(char prog [])
{
    // Attempts to find prog in PATH.
    // Returns the first path in PATH for prog.
    // If prog is not found in any path in PATH, returns NOT_FOUND
    char *ret_buffer = (char *)malloc(MAX_LINE_LEN);
    char *paths = (char *)malloc(MAX_PATH_LEN);

    // Get PATH environment variable and local path for paths
    snprintf(paths, MAX_PATH_LEN * sizeof(char), "%s:%s", getenv("PATH"), ".");
    char *path = strtok(paths, ":");

    while (path != NULL)
    {
        // Return the path where the executable is first encountered
        if (checkFileInDir(path, prog))
        {
            snprintf(ret_buffer, MAX_LINE_LEN * sizeof(char), "%s/%s", path, prog);
            free(paths);
            return ret_buffer;
        }
        path = strtok(NULL, ":");
    }

    snprintf(ret_buffer, MAX_LINE_LEN * sizeof(char), NOT_FOUND);
    free(paths);
    return ret_buffer;
}

// Some code adapted from: http://stackoverflow.com/questions/4204666/
bool checkFileInDir(char *path, char *file)
{
    // If file is present in path, return true, otherwise false
    // Usage example: checkFileInDir("/bin", "ls");
    DIR *directory;
    struct dirent *dir_entry;
    directory = opendir(path);
    bool res = false;

    // If directory exists, continuously read its contents. If the name of one of the contents matches file, return true.
    if (directory)
    {
        // Each iteration, get a directory entry
        while ((dir_entry = readdir(directory)) != NULL)
        {
            if (!strcmp(file, dir_entry->d_name))
            {
                res = true;
                break;
            }
        }
        closedir(directory);
    }

    return res;
}
