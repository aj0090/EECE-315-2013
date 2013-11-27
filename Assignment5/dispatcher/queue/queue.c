/**
 * Queue
 */

#include "queue.h"

//EFFECTS: Returns a handle to a new queue;
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

//REQUIRES: queuePtr is a valid pointer to a Queue, currentProcess is a valid pointer to a process
//MODIFIES: queuePtr
//EFFECTS: currentProcess is encapsulated in a Queue data structure, and placed in tail-end of Queue
void enqueueProcess(Queue *queuePtr, PCB *currentProcess) {
        if (!queuePtr->isHead) {
                queuePtr->process = currentProcess;
                queuePtr->isHead = 1;
        } else {
                Queue *iter = queuePtr;
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

//REQUIRES: queuePtrAddress is a valid address of a Queue pointer
//MODIFIES: *queuePtrAddress
//EFFECTS: The head of the queue at queuePtrAddress/NULL is returned, and the queue has its first element popped
Queue* dequeueProcess(Queue **queuePtrAddress) {
        if(!((*queuePtrAddress)->process)) {
                printf("No more processes left returning NULL\n");
                return NULL;
        }



        Queue *tmp = *queuePtrAddress;
	if((*queuePtrAddress)->next == NULL){
		*queuePtrAddress = NULL;			
		printf("SHIT\n");
		//return NULL;	
	}
	else {
        	(*queuePtrAddress) = (*queuePtrAddress)->next;
	}

       // if (*queuePtrAddress)
         //       (*queuePtrAddress)->isHead = 1;

        //tmp->next = NULL;

        return tmp;
}

//REQUIRES: queuePtrAddress is a valid address of a Queue pointer, newHead is a valid pointer to a Queue type of 1 element
//MODIFIES: *queuePtrAddress
//EFFECTS: The head of the queue at queuePtrAddress becomes newHead
void pushbackQueueElement(Queue **queuePtrAddress, Queue *newHead) {
        if (!(*queuePtrAddress)) {
                *queuePtrAddress = newHead;
                (*queuePtrAddress)->isHead = 1;
        }
        else if ((*queuePtrAddress)->isHead) {
                (*queuePtrAddress)->isHead = 0;
                Queue *tmp = *queuePtrAddress;
                *queuePtrAddress = newHead;
                (*queuePtrAddress)->isHead = 1;
                (*queuePtrAddress)->next = tmp;
        } else {
                fprintf(stderr, "Attemping to push element to a position not at the front of a queue\n");
        }
        return;
}

//REQUIRES: queueElement is a valid poitner to a queue of 1 element, and does not need to be used again
//MODIFIES: queueElement
//EFFECTS: The memory used by queueElement and its process is freed;
void terminateQueueElement(Queue *queueElement) {
        free(queueElement->process);
        free(queueElement);
}

//REQUIRES: queuePtr is a valid poitner to a queue
//MODIFIES: queuPtr
//EFFECTS: The memory used by queuePtr and its held processes are freed;
void cleanQueue(Queue *queuePtr) {

        while (queuePtr) {
                Queue *tmp = queuePtr->next;
                free(queuePtr->process);
                free(queuePtr);
                queuePtr = tmp;
        }
}

int numElems(Queue *queuePtr) {
        if (!(queuePtr)) {
                printf("The queue is empty!\n\n");
                return 0;
        }

	int count = 1;
        while (queuePtr->next) {
                count++;
                queuePtr = queuePtr->next;
        }
        return count;
}

//NOTE*** Used for debugging purposes if process is of type *int, comment out if using other process type
//REQUIRES: queuePtr is a valid poitner to a queue
//EFFECTS: Returns information about the queue;
void printQueue(Queue *queuePtr) {
        if (!(queuePtr) || queuePtr->process == NULL) {
                printf("The queue is empty!\n\n");
                return;
        }

        printf("Order of PID's: %d", queuePtr->process->pid);
        while (queuePtr->next) {
                printf(", %d", queuePtr->next->process->pid);
                queuePtr = queuePtr->next;
        }
        printf("\n\n");
}
