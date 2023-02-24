/**
 * CS2106 AY22/23 Semester 2 - Lab 2
 *
 * This file contains function definitions. Your implementation should go in
 * this file.
 */

#include "myshell.h"

#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define ARR_SIZE 50
#define NOT_IN_USE -1
static struct PCBTable processArr[ARR_SIZE];

/*******************************************************************************
 * Signal handler : ex4
 ******************************************************************************/

static void signal_handler(int signo)
{

    // Use the signo to identy ctrl-Z or ctrl-C and print “[PID] stopped or print “[PID] interrupted accordingly.
    // Update the status of the process in the PCB table
}

static void proc_update_status(/* pass necessary parameters*/)
{

    /******* FILL IN THE CODE *******/

    // Call everytime you need to update status and exit code of a process in PCBTable

    // May use WIFEXITED, WEXITSTATUS, WIFSIGNALED, WTERMSIG, WIFSTOPPED
}

/*******************************************************************************
 * Built-in Commands
 ******************************************************************************/

static void command_info(char *n)
{
    // checks for NULL as strcmp will terminate on NULL
    if (!n)
    {
        printf("Wrong command\n");
        return;
    }

    int counter = 0;
    if (strcmp(n, "0") == 0)
    {
        // status logging
        for (int i = 0; i < ARR_SIZE; i++)
        {
            if (processArr[i].status != NOT_IN_USE)
            {
                if (processArr[i].status == 1)
                {
                    // exit
                    printf("[%d] %s, %d", processArr[i].pid, "Exited", processArr[i].status);
                }
                else if (processArr[i].status == 2)
                {
                    // running
                    printf("[%d] %s", processArr[i].pid, "Running");
                }
            }
        }
    }
    else if (strcmp(n, "1") == 0)
    {
        // exited processes
        for (int i = 0; i < ARR_SIZE; i++)
        {
            if (processArr[i].status == 1)
            {
                counter++;
            }
        }
        printf("Total exited process: %d\n", counter);
    }
    else if (strcmp(n, "2") == 0)
    {
        // running processes
        for (int i = 0; i < ARR_SIZE; i++)
        {
            if (processArr[i].status == 2)
            {
                counter++;
            }
        }
        printf("Total running process: %d\n", counter);
    }
    else
    {
        // invalid args
        printf("Wrong command\n");
    }
}

static void command_wait(/* pass necessary parameters*/)
{

    /******* FILL IN THE CODE *******/

    // Find the {PID} in the PCBTable
    // If the process indicated by the process id is RUNNING, wait for it (can use waitpid()).
    // After the process terminate, update status and exit code (call proc_update_status())
    // Else, continue accepting user commands.
}

static void command_terminate(/* pass necessary parameters*/)
{

    /******* FILL IN THE CODE *******/

    // Find the pid in the PCBTable
    // If {PID} is RUNNING:
    // Terminate it by using kill() to send SIGTERM
    // The state of {PID} should be “Terminating” until {PID} exits
}

static void command_fg(/* pass necessary parameters*/)
{

    /******* FILL IN THE CODE *******/

    // if the {PID} status is stopped
    // Print “[PID] resumed”
    // Use kill() to send SIGCONT to {PID} to get it continue and wait for it
    // After the process terminate, update status and exit code (call proc_update_status())
}

/*******************************************************************************
 * Program Execution
 ******************************************************************************/

static void command_exec(/* pass necessary parameters*/)
{

    /******* FILL IN THE CODE *******/

    // check if program exists and is executable : use access()

    // fork a subprocess and execute the program

    pid_t pid;
    if ((pid = fork()) == 0)
    {
        // CHILD PROCESS

        // check file redirection operation is present : ex3

        // if < or > or 2> present:
        // use fopen/open file to open the file for reading/writing with  permission O_RDONLY, O_WRONLY, O_CREAT, O_TRUNC, O_SYNC and 0644
        // use dup2 to redirect the stdin, stdout and stderr to the files
        // call execv() to execute the command in the child process

        // else : ex1, ex2
        // call execv() to execute the command in the child process

        // Exit the child
    }
    else
    {

        // PARENT PROCESS
        // register the process in process table

        // If  child process need to execute in the background  (if & is present at the end )
        // print Child [PID] in background

        // else wait for the child process to exit

        // Use waitpid() with WNOHANG when not blocking during wait and  waitpid() with WUNTRACED when parent needs to block due to wait
    }
}

/*******************************************************************************
 * Command Processor
 ******************************************************************************/

static void command(/* pass necessary parameters*/)
{

    /******* FILL IN THE CODE *******/

    // if command is "info" call command_info()             : ex1
    // if command is "wait" call command_wait()             : ex2
    // if command is "terminate" call command_terminate()   : ex2
    // if command is "fg" call command_fg()                 : ex4

    // call command_exec() for all other commands           : ex1, ex2, ex3
}

/*******************************************************************************
 * High-level Procedure
 ******************************************************************************/

void my_init(void)
{
    // use signal() with SIGTSTP to setup a signalhandler for ctrl+z : ex4
    // use signal() with SIGINT to setup a signalhandler for ctrl+c  : ex4

    // anything else you require

    for (int iterator = 0; iterator < ARR_SIZE; iterator++)
    {
        processArr[iterator].status = NOT_IN_USE;
    }
}

void my_process_command(size_t num_tokens, char **tokens)
{
    // Split tokens at NULL or ; to get a single command (ex1, ex2, ex3, ex4(fg command))

    // for example :  /bin/ls ; /bin/sleep 5 ; /bin/pwd
    // split the above line as first command : /bin/ls , second command: /bin/pwd  and third command:  /bin/pwd
    // Call command() and pass each individual command as arguements

    // initialize PCB array
    my_init();

    // parse tokens
    int iterator = 0;
    while ((size_t)iterator < num_tokens)
    {
        // doing strcmp on NULL tokens will cause a force exit
        if (tokens[iterator] && strcmp(tokens[iterator], "info") == 0)
        {
            // advance to arg after info
            iterator++;
            command_info(tokens[iterator]);
        }
        iterator++;
    }

    // debug tools
    // int iterator;
    // for (iterator = 0; iterator < num_tokens; iterator++)
    // {
    //     printf("Token: %s\n", tokens[iterator]);
    // }
}

void my_quit(void)
{

    /******* FILL IN THE CODE *******/
    // Kill every process in the PCB that is either stopped or running

    printf("\nGoodbye\n");
}
