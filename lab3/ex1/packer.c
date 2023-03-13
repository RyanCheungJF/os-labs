#include "packer.h"

#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>

// constants
#define RED 1
#define GREEN 2
#define BLUE 3
// semaphore declarations
sem_t mutex;
sem_t redBallSemaphore;
sem_t greenBallSemaphore;
sem_t blueBallSemaphore;
int redBalls;
int greenBalls;
int blueBalls;
int redIds[2] = {-1, -1};
int greenIds[2] = {-1, -1};
int blueIds[2] = {-1, -1};

void packer_init(void)
{
    // initialize semaphores to be used with threads
    // sem_init takes in 3 args, name, 0 if shared w threads 1 for processes, init value
    sem_init(&mutex, 0, 1);
    sem_init(&redBallSemaphore, 0, 0);
    sem_init(&greenBallSemaphore, 0, 0);
    sem_init(&blueBallSemaphore, 0, 0);
    redBalls = 0;
    greenBalls = 0;
    blueBalls = 0;
}

void packer_destroy(void)
{
    // deinitialize any variables
    sem_destroy(&mutex);
    sem_destroy(&redBallSemaphore);
    sem_destroy(&greenBallSemaphore);
    sem_destroy(&blueBallSemaphore);
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
        if (redIds[0] == -1)
        {
            redIds[0] = id;
        }
        else
        {
            redIds[1] = id;
        }
        // only the second ball triggers this
        if (redBalls == 2)
        {
            // printf("TRIGGERED %d, %d balls\n", id, redBalls);
            trigger = true;
            redBalls = 0;
            if (redIds[0] == id)
            {
                res = redIds[1];
                redIds[1] = -1;
            }
            else
            {
                res = redIds[0];
                redIds[0] = -1;
            }
            sem_post(&redBallSemaphore);
        }
        sem_post(&mutex);

        if (trigger)
        {
            return res;
        }

        // printf("BEFORE BLOCKED %d, %d balls, arr %d %d\n", id, redBalls, redIds[0], redIds[1]);
        //  first ball blocks here
        sem_wait(&redBallSemaphore);
        // stores the other id
        sem_wait(&mutex);
        if (redIds[0] == -1)
        {
            res = redIds[1];
            redIds[1] = -1;
        }
        else
        {
            res = redIds[0];
            redIds[0] = -1;
        }
        sem_post(&mutex);
        // printf("AFTER BLOCKED %d, %d balls, arr %d %d\n", id, redBalls, redIds[0], redIds[1]);
        return res;
    }
    else if (colour == GREEN)
    {
        // green ball
        sem_wait(&mutex);
        greenBalls++;
        // save ids
        if (greenIds[0] == -1)
        {
            greenIds[0] = id;
        }
        else
        {
            greenIds[1] = id;
        }
        // only the second ball triggers this
        if (greenBalls == 2)
        {
            // printf("TRIGGERED %d, %d balls\n", id, redBalls);
            trigger = true;
            greenBalls = 0;
            if (greenIds[0] == id)
            {
                res = greenIds[1];
                greenIds[1] = -1;
            }
            else
            {
                res = greenIds[0];
                greenIds[0] = -1;
            }
            sem_post(&greenBallSemaphore);
        }
        sem_post(&mutex);

        if (trigger)
        {
            return res;
        }

        // printf("BEFORE BLOCKED %d, %d balls, arr %d %d\n", id, redBalls, redIds[0], redIds[1]);
        //  first ball blocks here
        sem_wait(&greenBallSemaphore);
        // stores the other id
        sem_wait(&mutex);
        if (greenIds[0] == -1)
        {
            res = greenIds[1];
            greenIds[1] = -1;
        }
        else
        {
            res = greenIds[0];
            greenIds[0] = -1;
        }
        sem_post(&mutex);
        // printf("AFTER BLOCKED %d, %d balls, arr %d %d\n", id, redBalls, redIds[0], redIds[1]);
        return res;
    }
    else if (colour == BLUE)
    {
        // blue ball
        sem_wait(&mutex);
        blueBalls++;
        // save ids
        if (blueIds[0] == -1)
        {
            blueIds[0] = id;
        }
        else
        {
            blueIds[1] = id;
        }
        // only the second ball triggers this
        if (blueBalls == 2)
        {
            // printf("TRIGGERED %d, %d balls\n", id, redBalls);
            trigger = true;
            blueBalls = 0;
            if (blueIds[0] == id)
            {
                res = blueIds[1];
                blueIds[1] = -1;
            }
            else
            {
                res = blueIds[0];
                blueIds[0] = -1;
            }
            sem_post(&blueBallSemaphore);
        }
        sem_post(&mutex);

        if (trigger)
        {
            return res;
        }

        // printf("BEFORE BLOCKED %d, %d balls, arr %d %d\n", id, redBalls, redIds[0], redIds[1]);
        //  first ball blocks here
        sem_wait(&blueBallSemaphore);
        // stores the other id
        sem_wait(&mutex);
        if (blueIds[0] == -1)
        {
            res = blueIds[1];
            blueIds[1] = -1;
        }
        else
        {
            res = blueIds[0];
            blueIds[0] = -1;
        }
        sem_post(&mutex);
        // printf("AFTER BLOCKED %d, %d balls, arr %d %d\n", id, redBalls, redIds[0], redIds[1]);
        return res;
    }
    else
    {
        // error
        printf("Colour of ball not recognized, got colour %d and id %d!\n", colour, id);
    }
    return 0;
}