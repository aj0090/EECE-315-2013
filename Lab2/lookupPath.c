#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

int lookupPath(char prog []) {

    char *path = getenv("PATH");
    char *paths = strtok(path, ":");

    while (paths != NULL) {
        printf("%s\n", paths);
        paths = strtok(NULL, ":");
    }


}

bool check_file_in_dir(char *path, char *file) {
    // Some code adapted from: http://stackoverflow.com/questions/4204666/
    DIR *d;
    struct dirent *dir;
    d = opendir(path);

    if (d) {
        while ((dir = readdir(d)) != NULL) {
            printf("%s\n", dir->d_name);
            if (!strcmp(file, dir->d_name)) {
                return true;
            }
        }
        closedir(d);
    }
    return false;
}



void main(void) {
    lookupPath("ls");
    check_file_in_dir(".", "lookupPath.c");
}

