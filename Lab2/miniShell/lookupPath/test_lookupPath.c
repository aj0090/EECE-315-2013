#include "lookupPath.h"

void main(void) {
    char prog [80];

    printf("Test for lookupPath\nEnter executable name: ");
    scanf("%s", prog);
    printf("Path for %s is %s\n", prog, lookupPath(prog));
}
