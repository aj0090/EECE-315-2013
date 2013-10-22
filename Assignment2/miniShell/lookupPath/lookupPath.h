#ifndef LOOKUPPATH_H
#define LOOKUPPATH_H

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Constants
#define MAX_LINE_LEN    80
#define MAX_PATH_LEN    1000
#define NOT_FOUND       "FILENOTFOUND"

// Function Prototypes
bool checkFileInDir(char *path, char *file);
char *lookupPath(char prog []);

#endif
