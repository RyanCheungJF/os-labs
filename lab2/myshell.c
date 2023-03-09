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
            if (WIFSIGNALED(status))
            {
                proc_update_status(p, EXITED, WTERMSIG(status));
            }
            else
            {
                proc_update_status(p, EXITED, 0);
            }
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
        // terminating processes
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
}

static void command_terminate(char *n)
{
    // convert string to int
    int p = atoi(n);
    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        if (p == processArr[i].pid && processArr[i].status == RUNNING)
        {
            // send SIGTERM with kill() to terminate, SIGTERM = SIGnal + TERMinate
            kill(p, SIGTERM);
            processArr[i].status = TERMINATING;
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
        bool in = false, out = false, err = false;
        char inputFileName[64], outputFileName[64], errorFileName[64];
        // checks for file redirection
        for (int i = 0; i < num_tokens; i++)
        {
            if (argList[i] && strcmp(argList[i], "<") == 0)
            {
                argList[i] = NULL;
                // copy file name of STDIN
                strcpy(inputFileName, argList[i + 1]);
                in = true;
            }
            if (argList[i] && strcmp(argList[i], ">") == 0)
            {
                argList[i] = NULL;
                // copy file name of STDOUT
                strcpy(outputFileName, argList[i + 1]);
                out = true;
            }
            if (argList[i] && strcmp(argList[i], "2>") == 0)
            {
                argList[i] = NULL;
                // copy file name of STDERR
                strcpy(errorFileName, argList[i + 1]);
                err = true;
            }
        }

        // if STDIN is present, change file descriptors
        if (in)
        {
            int fd;
            // check if we file exists to read from
            if (access(inputFileName, F_OK) == -1)
            {
                printf("%s does not exist\n", inputFileName);
                // exit with error code 1
                _exit(1);
            }
            fd = open(inputFileName, O_RDONLY);
            dup2(fd, STDIN_FILENO);
            close(fd);
        }
        // if STDOUT is present, change file descriptors
        if (out)
        {
            int fd;
            // create file we want to write too
            // 0644 flag means user can read and write, group and others can read
            fd = creat(outputFileName, 0644);
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }
        // if STDERR is present, change file descriptors
        if (err)
        {
            int fd;
            // create file we want to write too
            // 0644 flag means user can read and write, group and others can read
            fd = creat(errorFileName, 0644);
            dup2(fd, STDERR_FILENO);
            close(fd);
        }

        // execute as per normal
        execv(filePath, argList);
        // exit child process
        // 0 indicates successful exit
        _exit(0);
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
    }
}

/*******************************************************************************
 * Command Processor
 ******************************************************************************/

static void command(size_t num_tokens, char **tokens)
{
    // clean up at the start of a new command
    clean_up_proc();
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
            int index = 0;
            while ((size_t)iterator < num_tokens - 1)
            {
                // check for chained commands
                while ((size_t)iterator < num_tokens - 1 && strcmp(tokens[iterator], ";") != 0)
                {
                    argList[index] = tokens[iterator];
                    iterator++;
                    index++;
                }
                // we've now parsed a single whole command, so send it over
                command_exec(index, argList);
                // reset the array
                for (int i = 0; i < index; i++)
                {
                    argList[i] = NULL;
                }
                index = 0;
                iterator++;
            }
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
            // SIGKILL for immediate termination
            kill(processArr[i].pid, SIGKILL);
            printf("Killing [%d]\n", processArr[i].pid);
        }
    }
    printf("\nGoodbye\n");
}
