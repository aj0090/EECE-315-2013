/* Message format: "{Fork number}{Philosopher Number}{Picking up/Putting Down (1/0)}" */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <zmq.h>
#include "constants.h"


//Function prototypes
void *philosopher(int n);
int rightfork(int id);
int leftfork(int id);
void getLeftFork(int id);
void getRightFork(int id);
void return_forks(int n, int mealsEaten);
void pickup_forks(int n);


//Fork struct
typedef struct fork
{
    pthread_mutex_t lock;
} frk;


//Initialize threads and forks
pthread_t p[NPHILOSOPHERS];
frk f[NPHILOSOPHERS];

void *sock;
pthread_mutex_t sock_lock;


int main ( void )
{
    int i;

    // Set up socket to talk to anim.py
    void *context = zmq_ctx_new();
    sock  = zmq_socket(context, ZMQ_REP);
    int rc = zmq_bind(sock, SERV_ADDR);
    assert (rc == 0); // assert that bind was successful

    //Set up fork structs
    for (i = 0; i < NPHILOSOPHERS; ++i)
    {
        pthread_mutex_init(&f[i].lock, NULL); // initialize mutex
    }
    pthread_mutex_init(&sock_lock, NULL);

    //Create threads, join threads, and then destroy mutexes before quitting
    for (i = 0; i < NPHILOSOPHERS; ++i)
    {
        pthread_create(&p[i], NULL, (void *)philosopher, (int *)i);
    }
    for (i = 0; i < NPHILOSOPHERS; ++i)
    {
        pthread_join(p[i], NULL);
    }
    for (i = 0; i < NPHILOSOPHERS; ++i)
    {
        pthread_mutex_destroy(&f[i].lock);
    }
    pthread_mutex_destroy(&sock_lock);

    return 0;
}


void *philosopher(int n)
{
    int mealsEaten;

    // Eat until I've finished eating NMEALS
    for (mealsEaten = 0; mealsEaten < NMEALS; ++mealsEaten)
    {
        //Thinking...
        usleep(ACTIME);

        pickup_forks(n);

        if (VERBOSE)printf("\nI, Philosopher %d, am eating meal %d.", n, mealsEaten);
        //Eating...
        usleep(ACTIME);

        //Put forks down
        return_forks(n, mealsEaten);
    }

    //Finish up and leave
    if (VERBOSE)printf("\nI, Philosopher %d, thoroughly enjoyed my meals and now must depart.", n);
    if (VERBOSE)printf("\n");
}

void pickup_forks(int n)
{
    //Check for the lower numbered fork and grab it first
    if (leftfork(n) > rightfork(n))
    {
        getLeftFork(n);
        getRightFork(n);
    }
    else
    {
        getRightFork(n);
        getLeftFork(n);
    }
}

void getLeftFork(int n)
{
    char buffer[10];
    char msg[3];
    sprintf(msg, "%d%d1", leftfork(n), n);

    if (VERBOSE)printf("\nI, Philosopher %d, am attempting to grab my left fork.", n);
    pthread_mutex_lock(&f[leftfork(n)].lock);

    pthread_mutex_lock(&sock_lock);
    zmq_recv(sock, buffer, 10, 0);
    zmq_send(sock, msg, 3, 0);
    pthread_mutex_unlock(&sock_lock);

    if (VERBOSE)printf("\nI, Philosopher %d, have grabbed my left fork.", n);
}

void getRightFork(int n)
{
    char buffer[10];
    char msg[3];
    sprintf(msg, "%d%d1", rightfork(n), n);

    if (VERBOSE)printf("\nI, Philosopher %d, am attempting to grab my right fork.", n);
    pthread_mutex_lock(&f[rightfork(n)].lock);

    pthread_mutex_lock(&sock_lock);
    zmq_recv(sock, buffer, 10, 0);
    zmq_send(sock, msg, 3, 0);
    pthread_mutex_unlock(&sock_lock);

    if (VERBOSE)printf("\nI, Philosopher %d, have grabbed my right fork.", n);
}

void return_forks(int n, int mealsEaten)
{
    char buffer[10];
    char msg[3];

    pthread_mutex_lock(&sock_lock);
    sprintf(msg, "%d%d0", leftfork(n), n);
    zmq_recv(sock, buffer, 10, 0);
    zmq_send(sock, msg, 3, 0);
    pthread_mutex_unlock(&f[leftfork(n)].lock);


    sprintf(msg, "%d%d0", rightfork(n), n);
    zmq_recv(sock, buffer, 10, 0);
    zmq_send(sock, msg, 3, 0);
    pthread_mutex_unlock(&f[rightfork(n)].lock);
    pthread_mutex_unlock(&sock_lock);


    if (VERBOSE)printf("\nI, Philosopher %d, am finished eating meal %d.", n, mealsEaten);
}

//Returns the left fork id for philosopher #id
int leftfork(int id)
{
    return id;
}

//Returns the right fork id for philosopher #id
int rightfork(int id)
{
    return (id + 1) % NPHILOSOPHERS;
}
