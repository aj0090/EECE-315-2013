# EECE 315 101 2013

A repository for code used for EECE 315 101 2013.

[![Build Status](https://travis-ci.org/tonglil/EECE-315-2013.svg)](https://travis-ci.org/tonglil/EECE-315-2013)

## Links

[Trello](https://trello.com/b/B5YVE1uU/eece-315-group-12)

[Google Docs](https://drive.google.com/folderview?id=0B5oHB38MvjcMOWV6MUhWM3BjRUE)

## Building

### Make

```makefile
make: *.c
    gcc -g -o ProgramName fileX.c fileY.c fileZ.c
```

### .h

```c
#ifndef HEADER_FILE_H
#define HEADER_FILE_H

// Includes
#include <stdio.h>
#include "custom.h"

// Constants
#define THIS    0

// Define VERBOSE to 1 for debugging outputs
#define VERBOSE 0

// Struct definitions

// Function Prototypes
void function(void);

#endif
```

## Valgrind

NOTE: currently incompatible with Mac OS X 10.9.

"definitely lost" means your program is leaking memory -- fix those leaks!

"indirectly lost" means your program is leaking memory in a pointer-based structure. (E.g. if the root node of a binary tree is "definitely lost", all the children will be "indirectly lost".) If you fix the "definitely lost" leaks, the "indirectly lost" leaks should go away.

"possibly lost" means your program is leaking memory, unless you're doing unusual things with pointers that could cause them to point into the middle of an allocated block; see the user manual for some possible causes. Use --show-possibly-lost=no if you don't want to see these reports.

"still reachable" means your program is probably ok -- it didn't free some memory it could have. This is quite common and often reasonable. Don't use --show-reachable=yes if you don't want to see these reports.

"suppressed" means that a leak error has been suppressed. There are some suppressions in the default suppression files. You can ignore suppressed errors.

### .valgrindrc

Create a .valgrindrc file with these settings to simplify your life.

```
--memcheck:leak-check=full
--track-origins=yes
--show-reachable=yes
```

## Travis CI

Travis CI is a continuous integration service that can build and run tests on the repo.

You can skip a build by putting `[skip ci]` in the commit message.
