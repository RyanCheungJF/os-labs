#include "packer.h"

#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>

// constants
#define UNUSED -1
#define RED 1
#define GREEN 2
#define BLUE 3
// semaphore declarations
sem_t mutex;
sem_t redBarrierSemaphore;
sem_t greenBarrierSemaphore;
sem_t blueBarrierSemaphore;
sem_t redCountingSemaphore;
sem_t greenCountingSemaphore;
sem_t blueCountingSemaphore;
int redBalls;
int greenBalls;
int blueBalls;
static int redIds[2] = {UNUSED, UNUSED};
static int greenIds[2] = {UNUSED, UNUSED};
static int blueIds[2] = {UNUSED, UNUSED};

void packer_init(void)
{
    // initialize semaphores to be used with threads
    // sem_init takes in 3 args, name, 0 if shared w threads 1 for processes, init value
    sem_init(&mutex, 0, 1);
    sem_init(&redBarrierSemaphore, 0, 0);
    sem_init(&greenBarrierSemaphore, 0, 0);
    sem_init(&blueBarrierSemaphore, 0, 0);
    sem_init(&redCountingSemaphore, 0, 2);
    sem_init(&greenCountingSemaphore, 0, 2);
    sem_init(&blueCountingSemaphore, 0, 2);
    redBalls = 0;
    greenBalls = 0;
    blueBalls = 0;
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

void packer_set_state_array(int id, int *arr)
{
    // assumption that the arr we pass is always size 2
    // simply sets the id into an unused spot
    if (arr[0] == UNUSED)
    {
        arr[0] = id;
    }
    else
    {
        arr[1] = id;
    }
}

int packer_read_state_array(int id, int *arr)
{
    // assumption that the arr we pass is always size 2, so just find the other elem
    if (arr[0] == id)
    {
        return arr[1];
    }
    else
    {
        return arr[0];
    }
}

void packer_reset_state_array(int *arr)
{
    // simply resets the array after being used for the next batch
    arr[0] = UNUSED;
    arr[1] = UNUSED;
}

int pack_ball(int colour, int id)
{
    int res;
    if (colour == RED)
    {
        // red ball
        sem_wait(&redCountingSemaphore);
        sem_wait(&mutex);
        redBalls++;
        // save ids
        packer_set_state_array(id, redIds);

        // only the second ball triggers this barrier
        if (redBalls == 2)
        {
            sem_post(&redBarrierSemaphore);
        }
        sem_post(&mutex);

        //  first ball blocks here
        sem_wait(&redBarrierSemaphore);
        sem_wait(&mutex);
        // gets partner's id
        res = packer_read_state_array(id, redIds);
        redBalls--;
        if (redBalls == 0)
        {
            packer_reset_state_array(redIds);
            sem_post(&redCountingSemaphore);
            sem_post(&redCountingSemaphore);
        }
        else
        {
            sem_post(&redBarrierSemaphore);
        }
        sem_post(&mutex);
        // does not need to signal individual semaphore as we want to reset the barrier
        return res;
    }
    else if (colour == GREEN)
    {
        // green ball
        sem_wait(&greenCountingSemaphore);
        sem_wait(&mutex);
        greenBalls++;
        packer_set_state_array(id, greenIds);

        if (greenBalls == 2)
        {
            sem_post(&greenBarrierSemaphore);
        }
        sem_post(&mutex);

        sem_wait(&greenBarrierSemaphore);
        sem_wait(&mutex);
        res = packer_read_state_array(id, greenIds);
        greenBalls--;
        if (greenBalls == 0)
        {
            packer_reset_state_array(greenIds);
            sem_post(&greenCountingSemaphore);
            sem_post(&greenCountingSemaphore);
        }
        else
        {
            sem_post(&greenBarrierSemaphore);
        }
        sem_post(&mutex);
        return res;
    }
    else if (colour == BLUE)
    {
        // blue ball
        sem_wait(&blueCountingSemaphore);
        sem_wait(&mutex);
        blueBalls++;
        packer_set_state_array(id, blueIds);

        if (blueBalls == 2)
        {
            sem_post(&blueBarrierSemaphore);
        }
        sem_post(&mutex);

        sem_wait(&blueBarrierSemaphore);
        sem_wait(&mutex);
        res = packer_read_state_array(id, blueIds);
        blueBalls--;
        if (blueBalls == 0)
        {
            packer_reset_state_array(blueIds);
            sem_post(&blueCountingSemaphore);
            sem_post(&blueCountingSemaphore);
        }
        else
        {
            sem_post(&blueBarrierSemaphore);
        }
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
