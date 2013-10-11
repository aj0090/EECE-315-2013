EECE 315 101 2013
=================

A repository for code worked used in EECE 315 101 2013.

# Make

    make: *.c
        gcc -g -o out miniShell.c fileX.c fileY.c fileZ.c

# Valgrind

"definitely lost" means your program is leaking memory -- fix those leaks!

"indirectly lost" means your program is leaking memory in a pointer-based structure. (E.g. if the root node of a binary tree is "definitely lost", all the children will be "indirectly lost".) If you fix the "definitely lost" leaks, the "indirectly lost" leaks should go away.

"possibly lost" means your program is leaking memory, unless you're doing unusual things with pointers that could cause them to point into the middle of an allocated block; see the user manual for some possible causes. Use --show-possibly-lost=no if you don't want to see these reports.

"still reachable" means your program is probably ok -- it didn't free some memory it could have. This is quite common and often reasonable. Don't use --show-reachable=yes if you don't want to see these reports.

"suppressed" means that a leak error has been suppressed. There are some suppressions in the default suppression files. You can ignore suppressed errors.
    
## .valgrindrc

    --memcheck:leak-check=full
    --track-origins=yes
    --show-reachable=yes
