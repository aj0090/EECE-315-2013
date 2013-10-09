#ifndef LOOKUPPATH_H
#define LOOKUPPATH_H

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_LINE_LEN    80

bool checkFileInDir(char *path, char *file);
char *lookupPath(char prog []);

#endif
