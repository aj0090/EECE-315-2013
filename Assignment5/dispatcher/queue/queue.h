/**
 * Queue
 */

#ifndef QUEUE_H
#define QUEUE_H

#include <stdio.h>
#include <stdlib.h>

//Remove line when implementing actual data structure in queue
typedef int replaceMe;

typedef struct queue {
    int isHead;
    struct queue * next;
    replaceMe *process;
} Queue;

Queue* initializeQueue();

void enqueueProcess(Queue *currentQueue, replaceMe *currentProcess);

Queue* dequeueProcess(Queue **headPtr);

void pushbackQueueElement(Queue **currentQueuePtr, Queue *newHead);

void terminateQueueElement(Queue *queueElement);

void printQueue(Queue *currentQueue);

void cleanQueue(Queue *queue);

#endif
