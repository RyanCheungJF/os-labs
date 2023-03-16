#include "packer.h"

#include <semaphore.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/queue.h>

// constants
#define UNUSED -1
#define RED 1
#define GREEN 2
#define BLUE 3
#define BLACK 4

// self defined struct for our queue
typedef struct _entry
{
    int id;
    TAILQ_ENTRY(_entry)
    entries;
} entry;

// shared mutex
sem_t mutex;

// barrier semaphores to sync 2 threads
sem_t redBarrierSemaphore;
sem_t greenBarrierSemaphore;
sem_t blueBarrierSemaphore;
sem_t blackBarrierSemaphore;

// define shared queues and pointers to the heads
TAILQ_HEAD(redQ, _entry);
struct redQ redQHead;
TAILQ_HEAD(greenQ, _entry);
struct greenQ greenQHead;
TAILQ_HEAD(blueQ, _entry);
struct blueQ blueQHead;
TAILQ_HEAD(blackQ, _entry);
struct blackQ blackQHead;

void packer_init(void)
{
    // initialize semaphores to be used with threads
    sem_init(&mutex, 0, 1);
    sem_init(&redBarrierSemaphore, 0, 0);
    sem_init(&greenBarrierSemaphore, 0, 0);
    sem_init(&blueBarrierSemaphore, 0, 0);
    sem_init(&blackBarrierSemaphore, 0, 0);
    // queues
    TAILQ_INIT(&redQHead);
    TAILQ_INIT(&greenQHead);
    TAILQ_INIT(&blueQHead);
    TAILQ_INIT(&blackQHead);
}

void packer_destroy(void)
{
    // deinitialize any variables
    sem_destroy(&mutex);
    sem_destroy(&redBarrierSemaphore);
    sem_destroy(&greenBarrierSemaphore);
    sem_destroy(&blueBarrierSemaphore);
    sem_destroy(&blackBarrierSemaphore);
}

int pack_ball(int colour, int id)
{
    int res;
    if (colour == RED)
    {
        // red ball, find green
        sem_wait(&mutex);
        // our barrier semaphore acts as a counter for the elements in the queue
        sem_post(&redBarrierSemaphore);
        // we insert our ball into the queue
        entry *redBall = (entry *)malloc(sizeof(entry));
        redBall->id = id;
        TAILQ_INSERT_TAIL(&redQHead, redBall, entries);
        sem_post(&mutex);

        // here we are basically forced to wait until there is an element in the green queue
        // as we initially set the semaphore to 0 and it will only increase upon insertion
        sem_wait(&greenBarrierSemaphore);
        sem_wait(&mutex);
        // remove the ball
        entry *greenBall = TAILQ_FIRST(&greenQHead);
        TAILQ_REMOVE(&greenQHead, greenBall, entries);
        res = greenBall->id;
        // clear memory
        free(greenBall);
        sem_post(&mutex);
        return res;
    }
    else if (colour == GREEN)
    {
        // green ball, find red
        sem_wait(&mutex);
        sem_post(&greenBarrierSemaphore);
        entry *greenBall = (entry *)malloc(sizeof(entry));
        greenBall->id = id;
        TAILQ_INSERT_TAIL(&greenQHead, greenBall, entries);
        sem_post(&mutex);

        sem_wait(&redBarrierSemaphore);
        sem_wait(&mutex);
        entry *redBall = TAILQ_FIRST(&redQHead);
        TAILQ_REMOVE(&redQHead, redBall, entries);
        res = redBall->id;
        free(redBall);
        sem_post(&mutex);
        return res;
    }
    else if (colour == BLUE)
    {
        // blue ball, find black
        sem_wait(&mutex);
        sem_post(&blueBarrierSemaphore);
        entry *blueBall = (entry *)malloc(sizeof(entry));
        blueBall->id = id;
        TAILQ_INSERT_TAIL(&blueQHead, blueBall, entries);
        sem_post(&mutex);

        sem_wait(&blackBarrierSemaphore);
        sem_wait(&mutex);
        entry *blackBall = TAILQ_FIRST(&blackQHead);
        TAILQ_REMOVE(&blackQHead, blackBall, entries);
        res = blackBall->id;
        free(blackBall);
        sem_post(&mutex);
        return res;
    }
    else if (colour == BLACK)
    {
        // black ball, find blue
        sem_wait(&mutex);
        sem_post(&blackBarrierSemaphore);
        entry *blackBall = (entry *)malloc(sizeof(entry));
        blackBall->id = id;
        TAILQ_INSERT_TAIL(&blackQHead, blackBall, entries);
        sem_post(&mutex);

        sem_wait(&blueBarrierSemaphore);
        sem_wait(&mutex);
        entry *blueBall = TAILQ_FIRST(&blueQHead);
        TAILQ_REMOVE(&blueQHead, blueBall, entries);
        res = blueBall->id;
        free(blueBall);
        sem_post(&mutex);
        return res;
    }
    else
    {
        // error, not a colour we support
        printf("Colour of ball not recognized, got colour %d and id %d!\n", colour, id);
    }
    return 0;
}
