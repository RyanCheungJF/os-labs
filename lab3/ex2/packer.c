#include "packer.h"

#include <semaphore.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

// constants
#define UNUSED -1
#define RED 1
#define GREEN 2
#define BLUE 3
// shared mutex
sem_t mutex;
// barrier semaphores to sync N threads
sem_t redBarrierSemaphore;
sem_t greenBarrierSemaphore;
sem_t blueBarrierSemaphore;
// counting semaphores to ensure one batch of N processes first
sem_t redCountingSemaphore;
sem_t greenCountingSemaphore;
sem_t blueCountingSemaphore;
// rest of the variables/ arrays, assumption of N to be no larger than 64
int redBalls;
int greenBalls;
int blueBalls;
int numberOfBalls;
static int redIds[64];
static int greenIds[64];
static int blueIds[64];

void packer_init(int balls_per_pack)
{
    // initialize semaphores to be used with threads
    // sem_init takes in 3 args, name, 0 if shared w threads 1 for processes, init value
    sem_init(&mutex, 0, 1);
    sem_init(&redBarrierSemaphore, 0, 0);
    sem_init(&greenBarrierSemaphore, 0, 0);
    sem_init(&blueBarrierSemaphore, 0, 0);
    sem_init(&redCountingSemaphore, 0, balls_per_pack);
    sem_init(&greenCountingSemaphore, 0, balls_per_pack);
    sem_init(&blueCountingSemaphore, 0, balls_per_pack);
    redBalls = 0;
    greenBalls = 0;
    blueBalls = 0;
    numberOfBalls = balls_per_pack;
    for (int i = 0; i < balls_per_pack; i++)
    {
        redIds[i] = UNUSED;
        greenIds[i] = UNUSED;
        blueIds[i] = UNUSED;
    }
}

void packer_destroy(void)
{
    // deinitialize any variables
    sem_destroy(&mutex);
    sem_destroy(&redBarrierSemaphore);
    sem_destroy(&greenBarrierSemaphore);
    sem_destroy(&blueBarrierSemaphore);
    sem_destroy(&redCountingSemaphore);
    sem_destroy(&greenCountingSemaphore);
    sem_destroy(&blueCountingSemaphore);
}

void update_array_entry(int id, int *arr)
{
    for (int i = 0; i < numberOfBalls; i++)
    {
        if (arr[i] == UNUSED)
        {
            arr[i] = id;
            return;
        }
    }
}

void copy_array_entries(int id, int *src, int *tgt)
{
    int index = 0;
    for (int i = 0; i < numberOfBalls; i++)
    {
        if (src[i] == id)
        {
            continue;
        }
        tgt[index] = src[i];
        index++;
    }
}

void reset_array_entries(int *arr)
{
    for (int i = 0; i < numberOfBalls; i++)
    {
        arr[i] = UNUSED;
    }
}

void pack_ball(int colour, int id, int *other_ids)
{
    if (colour == RED)
    {
        // counting semaphore is to ensure if we need to process multiple N batches at the same time, there is no interleave
        // initially initialized to N, this ensures that multiple batches will have to wait
        sem_wait(&redCountingSemaphore);
        // obtain mutex to change shared state
        sem_wait(&mutex);
        redBalls++;
        update_array_entry(id, redIds);
        if (redBalls == numberOfBalls)
        {
            // here we release the barrier semaphore once we have N balls
            sem_post(&redBarrierSemaphore);
        }
        sem_post(&mutex);

        // N - 1 threads wait here until the barrier is released
        sem_wait(&redBarrierSemaphore);
        // here we obtain the mutex again to prevent queued threads to enter the previous part once the counting semaphore is reset
        sem_wait(&mutex);
        // each thread copies partner entries and has the responsibility to decrement counter
        copy_array_entries(id, redIds, other_ids);
        redBalls--;
        // if its the last ball which decremented, it has the responsibility to reset the shared array and counting semaphore
        if (redBalls == 0)
        {
            reset_array_entries(redIds);
            for (int i = 0; i < numberOfBalls; i++)
            {
                // reset the semaphore, this won't trigger queued threads as we protected this section with a mutex
                sem_post(&redCountingSemaphore);
            }
        }
        else
        {
            // we want to reset the barrier as well, so all processes except the last one increment it
            sem_post(&redBarrierSemaphore);
        }
        sem_post(&mutex);
    }
    else if (colour == GREEN)
    {
        sem_wait(&greenCountingSemaphore);
        sem_wait(&mutex);
        greenBalls++;
        update_array_entry(id, greenIds);
        if (greenBalls == numberOfBalls)
        {
            sem_post(&greenBarrierSemaphore);
        }
        sem_post(&mutex);

        sem_wait(&greenBarrierSemaphore);
        sem_wait(&mutex);
        copy_array_entries(id, greenIds, other_ids);
        greenBalls--;
        if (greenBalls == 0)
        {
            reset_array_entries(greenIds);
            for (int i = 0; i < numberOfBalls; i++)
            {
                sem_post(&greenCountingSemaphore);
            }
        }
        else
        {
            sem_post(&greenBarrierSemaphore);
        }
        sem_post(&mutex);
    }
    else if (colour == BLUE)
    {
        sem_wait(&blueCountingSemaphore);
        sem_wait(&mutex);
        blueBalls++;
        update_array_entry(id, blueIds);
        if (blueBalls == numberOfBalls)
        {
            sem_post(&blueBarrierSemaphore);
        }
        sem_post(&mutex);

        sem_wait(&blueBarrierSemaphore);
        sem_wait(&mutex);
        copy_array_entries(id, blueIds, other_ids);
        blueBalls--;
        if (blueBalls == 0)
        {
            reset_array_entries(blueIds);
            for (int i = 0; i < numberOfBalls; i++)
            {
                sem_post(&blueCountingSemaphore);
            }
        }
        else
        {
            sem_post(&blueBarrierSemaphore);
        }
        sem_post(&mutex);
    }
    else
    {
        // error, not a colour we support
        printf("Colour of ball not recognized, got colour %d and id %d!\n", colour, id);
    }
}
