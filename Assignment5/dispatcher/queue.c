/**
 * Queue
 */

#include "queue.h"

Queue* initializeQueue() {
        Queue *new;
        if((new = malloc(sizeof(Queue)))) {
                new->isHead = 0;
                new->next = NULL;
                new->process = NULL;
                return new;
        } else {
                fprintf(stderr, "Creation failed\n");
                return NULL;
        }
}

void enqueueProcess(Queue *currentQueue, replaceMe *currentProcess) {
        if (!currentQueue->isHead) {
                currentQueue->process = currentProcess;
                currentQueue->isHead = 1;
        } else {
                Queue *iter = currentQueue;
                while (iter->next) // traverse the list until the end
                        iter = iter->next;
                Queue *new = malloc(sizeof(Queue));
                iter->next = new;
                iter->next->isHead = 0;
                iter->next->next = NULL;
                iter->next->process = currentProcess;
        }
        return;
}

Queue* dequeueProcess(Queue **headPtr) {
        if(!((*headPtr)->process)) {
                printf("No more processes left\n");
                return NULL;
        }
                
        Queue *tmp = *headPtr;
        *headPtr = (*headPtr)->next;

        if (*headPtr)
                (*headPtr)->isHead = 1;

        tmp->next = NULL;

        return tmp;
}

void pushbackQueueElement(Queue **currentQueuePtr, Queue *newHead) {
        if (!(*currentQueuePtr)) {
                *currentQueuePtr = newHead;
                (*currentQueuePtr)->isHead = 1;
        }
        else if ((*currentQueuePtr)->isHead) {
                (*currentQueuePtr)->isHead = 0;
                Queue *tmp = *currentQueuePtr;
                *currentQueuePtr = newHead;
                (*currentQueuePtr)->isHead = 1;
                (*currentQueuePtr)->next = tmp;
        } else {
                fprintf(stderr, "Attemping to push element to a position not at the front of a queue\n");
        }
        return;
}

void terminateQueueElement(Queue *queueElement) {
        free(queueElement->process);
        free(queueElement);
}

void cleanQueue(Queue *queue) {

        while (queue) {
                Queue *tmp = queue->next;
                free(queue->process);
                free(queue);
                queue = tmp;
        }
}

void printQueue(Queue *currentQueue) {
        if (!(currentQueue)) {
                printf("The queue is empty!\n\n");
                return;
        }

        printf("The queue is: %d", *(currentQueue->process));
        while (currentQueue->next) {
                printf(", %d", *(currentQueue->next->process));
                currentQueue = currentQueue->next;
        }
        printf("\n\n");
}