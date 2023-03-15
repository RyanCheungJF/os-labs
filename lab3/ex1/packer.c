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

int packer_reset_state_array(int id, int *arr)
{
    // assumption that the arr we pass is always size 2
    // simply resets the array after finding the partner and returns the partner
    int res;
    if (arr[0] == id)
    {
        res = arr[1];
        arr[1] = UNUSED;
    }
    else
    {
        res = arr[0];
        arr[0] = UNUSED;
    }
    return res;
}

int pack_ball(int colour, int id)
{
    bool trigger = false;
    int res;
    if (colour == RED)
    {
        // red ball
        sem_wait(&mutex);
        redBalls++;
        // save ids
        packer_set_state_array(id, redIds);

        // only the second ball triggers this barrier
        if (redBalls == 2)
        {
            // only the second ball has the responsibility of resetting shared state
            trigger = true;
            redBalls = 0;
            // get partner ball's id
            res = packer_reset_state_array(id, redIds);
            sem_post(&redBarrierSemaphore);
        }
        sem_post(&mutex);

        if (trigger)
        {
            // only for the second ball to terminate as we already got our partner's id
            return res;
        }

        //  first ball blocks here
        sem_wait(&redBarrierSemaphore);
        sem_wait(&mutex);
        // gets partner's id
        res = packer_reset_state_array(UNUSED, redIds);
        sem_post(&mutex);
        // does not need to signal individual semaphore as we want to reset the barrier
        return res;
    }
    else if (colour == GREEN)
    {
        // green ball
        sem_wait(&mutex);
        greenBalls++;
        packer_set_state_array(id, greenIds);

        if (greenBalls == 2)
        {
            trigger = true;
            greenBalls = 0;
            res = packer_reset_state_array(id, greenIds);
            sem_post(&greenBarrierSemaphore);
        }
        sem_post(&mutex);

        if (trigger)
        {
            return res;
        }

        sem_wait(&greenBarrierSemaphore);
        sem_wait(&mutex);
        res = packer_reset_state_array(UNUSED, greenIds);
        sem_post(&mutex);
        return res;
    }
    else if (colour == BLUE)
    {
        // blue ball
        sem_wait(&mutex);
        blueBalls++;
        packer_set_state_array(id, blueIds);

        if (blueBalls == 2)
        {
            trigger = true;
            blueBalls = 0;
            res = packer_reset_state_array(id, blueIds);
            sem_post(&blueBarrierSemaphore);
        }
        sem_post(&mutex);

        if (trigger)
        {
            return res;
        }

        sem_wait(&blueBarrierSemaphore);
        sem_wait(&mutex);
        res = packer_reset_state_array(UNUSED, blueIds);
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
