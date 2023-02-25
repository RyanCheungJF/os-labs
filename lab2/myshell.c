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

#define NOT_EXITED -1
#define UNUSED 0
#define EXITED 1
#define RUNNING 2
#define TERMINATING 3
#define STOPPED 4
static struct PCBTable processArr[MAX_PROCESSES];

/*******************************************************************************
 * Signal handler : ex4
 ******************************************************************************/

static void signal_handler(int signo)
{

    // Use the signo to identy ctrl-Z or ctrl-C and print “[PID] stopped or print “[PID] interrupted accordingly.
    // Update the status of the process in the PCB table
}

static void proc_update_status(pid_t pid, int status, int exitCode)
{
    // for (int i = 0; i < 5; i++)
    // {
    //     printf("%d %d %d\n", processArr[i].pid, processArr[i].status, processArr[i].exitCode);
    // }

    int i = 0;
    while (i < MAX_PROCESSES && processArr[i].status != UNUSED)
    {
        if (processArr[i].pid == pid)
        {
            // update entry
            break;
        }
        // else find unused entry
        i++;
    }
    processArr[i].pid = pid;
    processArr[i].status = status;
    processArr[i].exitCode = exitCode;
}

static void clean_up_proc()
{
    int p, status;
    // clean up process if done
    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        if (processArr[i].pid != UNUSED && ((p = waitpid(processArr[i].pid, &status, WNOHANG)) == processArr[i].pid))
        {
            proc_update_status(p, EXITED, 0);
        }
    }
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

    clean_up_proc();

    int counter = 0;
    if (strcmp(n, "0") == 0)
    {
        // status logging
        for (int i = 0; i < MAX_PROCESSES; i++)
        {
            if (processArr[i].status == EXITED)
            {
                // exited process
                printf("[%d] %s %d\n", processArr[i].pid, "Exited", processArr[i].exitCode);
            }
            else if (processArr[i].status == RUNNING)
            {
                // running process
                printf("[%d] %s\n", processArr[i].pid, "Running");
            }
            else if (processArr[i].status == TERMINATING)
            {
                // terminating process
                printf("[%d] %s\n", processArr[i].pid, "Terminating");
            }
        }
    }
    else if (strcmp(n, "1") == 0)
    {
        // exited processes
        for (int i = 0; i < MAX_PROCESSES; i++)
        {
            if (processArr[i].status == EXITED)
            {
                counter++;
            }
        }
        printf("Total exited process: %d\n", counter);
    }
    else if (strcmp(n, "2") == 0)
    {
        // running processes
        for (int i = 0; i < MAX_PROCESSES; i++)
        {
            if (processArr[i].status == RUNNING)
            {
                counter++;
            }
        }
        printf("Total running process: %d\n", counter);
    }
    else if (strcmp(n, "3") == 0)
    {
        // running processes
        for (int i = 0; i < MAX_PROCESSES; i++)
        {
            if (processArr[i].status == TERMINATING)
            {
                counter++;
            }
        }
        printf("Total terminating process: %d\n", counter);
    }
    else
    {
        // invalid args
        printf("Wrong command\n");
    }
}

static void command_wait(char *n)
{
    // convert string to int
    int p = atoi(n);
    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        if (p == processArr[i].pid && processArr[i].status == RUNNING)
        {
            // block and wait
            int status;
            if (waitpid(p, &status, WUNTRACED) > 0)
            {
                // update PCB table with new status and exit code
                proc_update_status(p, EXITED, WEXITSTATUS(status));
            }
        }
    }

    /******* FILL IN THE CODE *******/

    // Find the {PID} in the PCBTable
    // If the process indicated by the process id is RUNNING, wait for it (can use waitpid()).
    // After the process terminate, update status and exit code (call proc_update_status())
    // Else, continue accepting user commands.
}

static void command_terminate(char *n)
{

    /******* FILL IN THE CODE *******/

    // Find the pid in the PCBTable
    // If {PID} is RUNNING:
    // Terminate it by using kill() to send SIGTERM
    // The state of {PID} should be “Terminating” until {PID} exits
    // convert string to int
    int p = atoi(n);
    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        if (p == processArr[i].pid && processArr[i].status == RUNNING)
        {
            // send SIGTERM with kill() to terminate
            int exitCode = kill(p, SIGTERM);
            processArr[i].status = TERMINATING;
            processArr[i].exitCode = exitCode;
        }
    }
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

static void command_exec(int num_tokens, char **argList)
{
    const char *filePath = argList[0];
    // check if program exists and is executable
    // F_OK indicates existence of file
    // R_OK indicates readable file
    // X_OK checks for execution permission bit
    if (access(filePath, F_OK & R_OK & X_OK) == -1)
    {
        printf("%s not found\n", filePath);
        return;
    }

    // checks if our list of args has an ampersand
    // ampersand used to indicate running background process
    // when passing it to execv, this might be an unrecognized arg, so we reset it to NULL
    bool ampersandFlag = (strcmp(argList[num_tokens - 1], "&") == 0);
    if (ampersandFlag)
    {
        argList[num_tokens - 1] = NULL;
    }

    // fork a subprocess and execute the program
    int status;
    pid_t pid;
    if ((pid = fork()) == 0)
    {
        // child process
        execv(filePath, argList);
        // exit child process
        // 0 indicates successful exit
        _exit(0);

        // check file redirection operation is present : ex3

        // if < or > or 2> present:
        // use fopen/open file to open the file for reading/writing with  permission O_RDONLY, O_WRONLY, O_CREAT, O_TRUNC, O_SYNC and 0644
        // use dup2 to redirect the stdin, stdout and stderr to the files
        // call execv() to execute the command in the child process
    }
    else
    {
        // parent process
        // register process in PCB table
        proc_update_status(pid, RUNNING, NOT_EXITED);

        if (ampersandFlag)
        {
            // let child run in background
            printf("Child [%d] in background\n", pid);
        }
        else
        {
            // we block and wait
            if (waitpid(pid, &status, WUNTRACED) > 0)
            {
                // update PCB table with new status and exit code
                proc_update_status(pid, EXITED, WEXITSTATUS(status));
            }
        }
        // Use waitpid() with WNOHANG when not blocking during wait and  waitpid() with WUNTRACED when parent needs to block due to wait
    }
}

/*******************************************************************************
 * Command Processor
 ******************************************************************************/

static void command(size_t num_tokens, char **tokens)
{
    // parse tokens
    int iterator = 0;
    // we dont handle NULL terminator
    while ((size_t)iterator < num_tokens - 1)
    {
        // doing strcmp on NULL tokens will cause a force exit
        if (strcmp(tokens[iterator], "info") == 0)
        {
            // advance to arg after info
            iterator++;
            command_info(tokens[iterator]);
            iterator++;
        }
        else if (strcmp(tokens[iterator], "wait") == 0)
        {
            iterator++;
            command_wait(tokens[iterator]);
            iterator++;
        }
        else if (strcmp(tokens[iterator], "terminate") == 0)
        {
            iterator++;
            command_terminate(tokens[iterator]);
            iterator++;
        }
        else
        {
            // asking to execute a program
            char *argList[50] = {NULL};
            int i = 0;
            while ((size_t)iterator < num_tokens - 1)
            {
                argList[i] = tokens[iterator];
                iterator++;
                i++;
            }
            // execv(argList[0], argList);
            command_exec(i, argList);
        }
    }
}

/*******************************************************************************
 * High-level Procedure
 ******************************************************************************/

void my_init(void)
{
    // use signal() with SIGTSTP to setup a signalhandler for ctrl+z : ex4
    // use signal() with SIGINT to setup a signalhandler for ctrl+c  : ex4
}

void my_process_command(size_t num_tokens, char **tokens)
{
    // Split tokens at NULL or ; to get a single command (ex1, ex2, ex3, ex4(fg command))

    // for example :  /bin/ls ; /bin/sleep 5 ; /bin/pwd
    // split the above line as first command : /bin/ls , second command: /bin/pwd  and third command:  /bin/pwd
    // Call command() and pass each individual command as arguements

    // initialize PCB array
    my_init();
    command(num_tokens, tokens);

    // debug tools
    // int iterator;
    // for (iterator = 0; iterator < num_tokens; iterator++)
    // {
    //     printf("Token: %s\n", tokens[iterator]);
    // }
}

void my_quit(void)
{
    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        // kill every process in the PCB that is either stopped or running
        if (processArr[i].status == 2 || processArr[i].status == 4)
        {
            kill(processArr[i].pid, SIGKILL);
            printf("Killing [%d]\n", processArr[i].pid);
        }
    }
    printf("\nGoodbye\n");
}
