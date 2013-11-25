/**
 * For testing queue.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include "queue.c"

/* Main function */
int main(int argc, char **argv) {

    Queue *realTime = initializeQueue();
    printf("Queue has been initialized!\n");
    Queue *currentQueueElement0, *currentQueueElement1,*currentQueueElement2,*currentQueueElement3,*currentQueueElement4;

    int i;
    for (i = 0; i < 5; i++) {
        //Sample of inserting into the queue
        replaceMe *process = malloc((sizeof(replaceMe)));
        (*process) = i;
        enqueueProcess(realTime, process);
        printf("Enqueueing %d\n", *process);
    }

    printQueue(realTime);

    //Sample of dequeueing and getting a handle of dequeued item
    currentQueueElement0 = dequeueProcess(&realTime);
    printf("Dequeuing %d from the queue\n", *(currentQueueElement0->process));
    printQueue(realTime);

    currentQueueElement1 = dequeueProcess(&realTime);
    printf("Dequeuing %d from the queue\n", *(currentQueueElement1->process));
    printQueue(realTime);

    currentQueueElement2 = dequeueProcess(&realTime);
    printf("Dequeuing %d from the queue\n", *(currentQueueElement2->process));
    printQueue(realTime);

    currentQueueElement3 = dequeueProcess(&realTime);
    printf("Dequeuing %d from the queue\n", *(currentQueueElement3->process));
    printQueue(realTime);

    currentQueueElement4 = dequeueProcess(&realTime);
    printf("Dequeuing %d from the queue\n", *(currentQueueElement4->process));
    printQueue(realTime);

    //Sample of pushing to the front of the queue and back of the queue
    pushbackQueueElement(&realTime, currentQueueElement2);
    printf("Pushing back %d\n", *(currentQueueElement2->process));
    printf("After pushing back \n");
    printQueue(realTime);

    replaceMe *process = malloc((sizeof(replaceMe)));
    (*process) = 5;
    enqueueProcess(realTime, process);
    printf("Enqueueing %d\n", *process);
    printQueue(realTime);

    pushbackQueueElement(&realTime, currentQueueElement4);
    printf("Pushing back %d\n", *(currentQueueElement4->process));
    printQueue(realTime);

    pushbackQueueElement(&realTime, currentQueueElement3);
    printf("Pushing back %d\n", *(currentQueueElement3->process));
    printQueue(realTime);

    process = malloc((sizeof(replaceMe)));
    (*process) = 6;
    enqueueProcess(realTime, process);
    printf("Enqueueing %d\n", *process);
    printQueue(realTime);

    pushbackQueueElement(&realTime, currentQueueElement1);
    printf("Pushing back %d\n", *(currentQueueElement1->process));
    printQueue(realTime);

    pushbackQueueElement(&realTime, currentQueueElement0);
    printf("Pushing back %d\n", *(currentQueueElement0->process));
    printQueue(realTime);

    //Sample of getting process at front of queue, using it and freeing the memory
    currentQueueElement0 = dequeueProcess(&realTime);
    printf("Dequeuing %d from the queue\n", *(currentQueueElement0->process));
    printQueue(realTime);

    //Do something with currentQueueElement0->process...
    printf("Doing something with %d \n", *(currentQueueElement0->process));

    printf("Done using process %d and now terminating it \n\n", *(currentQueueElement0->process));
    terminateQueueElement(currentQueueElement0);

    //Sample of getting process at front of queue and putting it back
    currentQueueElement0 = dequeueProcess(&realTime);
    printf("Dequeuing %d from the queue\n", *(currentQueueElement0->process));
    printQueue(realTime);

    //Do something with currentQueueElement0->process...

    pushbackQueueElement(&realTime, currentQueueElement0);
    printf("Not done using process %d and pushing it back\n", *(currentQueueElement0->process));
    printQueue(realTime);

    cleanQueue(realTime);
    printf("Done cleaning queue, valgrind should have no memory leaks\n");

    return 0;
}
