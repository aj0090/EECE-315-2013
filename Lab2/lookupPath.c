#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int lookupPath(char prog []) {



    /*DIR *d;
    struct dirent *dir;
    d = opendir(".")*/
    return 0;
}

char **get_paths() {
    int tokens, i = 0;
    char * path_array;

    //Use getenv to get the PATH environment variable
    char *path = getenv("PATH");
    //Find the occurences of ":" which split paths in PATH
    for (tokens = 0; path[tokens]; path[tokens]==":" ? tokens++: path++);

    path_array = (char *) calloc (tokens, 100*sizeof(char));

    char *paths = strtok(path, ":");
    while (paths != NULL) {
        path_array[i] = paths; i++;
        //printf("%s\n", paths);
        paths = strtok(NULL, ":");
    }

    for (i=0; i<tokens; i++) {
        printf("%s\n", path_array[i]);
    }

    return path_array;
}

void main() {
    int a=0;
    lookupPath("ls");
    get_paths();
}
