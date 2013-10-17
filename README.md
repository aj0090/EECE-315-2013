EECE 315 101 2013
=================

A repository for code used for EECE 315 101 2013.

[![Build Status](https://magnum.travis-ci.com/UBCEECE315/Group12-Repo.png?token=ZpicHWVRcsKnmG8qc6PB&branch=master)](https://magnum.travis-ci.com/UBCEECE315/Group12-Repo)

# Make

    make: *.c
        gcc -g -o ProgramName fileX.c fileY.c fileZ.c

# Valgrind

"definitely lost" means your program is leaking memory -- fix those leaks!

"indirectly lost" means your program is leaking memory in a pointer-based structure. (E.g. if the root node of a binary tree is "definitely lost", all the children will be "indirectly lost".) If you fix the "definitely lost" leaks, the "indirectly lost" leaks should go away.

"possibly lost" means your program is leaking memory, unless you're doing unusual things with pointers that could cause them to point into the middle of an allocated block; see the user manual for some possible causes. Use --show-possibly-lost=no if you don't want to see these reports.

"still reachable" means your program is probably ok -- it didn't free some memory it could have. This is quite common and often reasonable. Don't use --show-reachable=yes if you don't want to see these reports.

"suppressed" means that a leak error has been suppressed. There are some suppressions in the default suppression files. You can ignore suppressed errors.

## .valgrindrc

Create .valgrindrc with these settings to simplify your life.

    --memcheck:leak-check=full
    --track-origins=yes
    --show-reachable=yes

# Travis CI

Travis CI is a continuous integration service that can build and run tests on the repo.

Note: it is not free, thus:
- Builds of the program will only be run on the `stable` branch.
- You can skip a build on the `stable` branch by putting `[ci skip]` in the commit message.

Our builds are [here](https://magnum.travis-ci.com/UBCEECE315/Group12-Repo).
