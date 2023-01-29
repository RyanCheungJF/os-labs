/*************************************
 * Lab 1 Exercise 3
 * Name: Ryan Cheung Jing Feng
 * Student No: A0217587R
 * Lab Group: 18
 *************************************/

#include <stdio.h>
#include <stdlib.h>

#include "function_pointers.h"
#include "node.h"

// The runner is empty now! Modify it to fulfill the requirements of the
// exercise. You can use ex2.c as a template

// DO NOT initialize the func_list array in this file. All initialization
// logic for func_list should go into function_pointers.c.

// Macros
#define SUM_LIST 0
#define INSERT_AT 1
#define DELETE_AT 2
#define SEARCH_LIST 3
#define REVERSE_LIST 4
#define RESET_LIST 5
#define LIST_LEN 6
#define MAP 7

// void run_instruction(list *lst, int instr);
void run_instruction(FILE *file, list *lst, int instr);
void print_sum(long sum);
void print_length(int len);
void print_index(int index);

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Error: expecting 1 argument, %d found\n", argc - 1);
        exit(1);
    }

    // We read in the file name provided as argument
    char *fileName = argv[1];

    // handle invalid filename
    FILE *file = fopen(fileName, "r"); // read mode
    if (file == NULL)
    {
        fprintf(stderr, "Error: invalid file name!\n");
        exit(1);
    }

    // Update the array of function pointers
    // DO NOT REMOVE THIS CALL
    // (You may leave the function empty if you do not need it)
    update_functions();

    // Rest of code logic here
    list *lst = (list *)malloc(sizeof(list));
    lst->head = NULL;

    int instr;
    /*
    while (fscanf(file, "%d", &instr) == 1)
    {
        run_instruction(lst, instr);
    }
    */
    while (fscanf(file, "%d", &instr) == 1)
    {
        run_instruction(file, lst, instr);
    };

    fclose(file);
    reset_list(lst);
    free(lst);
}

// Takes an instruction enum and runs the corresponding function
// We assume input always has the right format (no input validation on runner)
void run_instruction(FILE *file, list *lst, int instr)
{
    int index, data, element;
    switch (instr)
    {
    case SUM_LIST:; // semi colon to not treat this as label
        long sum = sum_list(lst);
        print_sum(sum);
        break;
    case INSERT_AT:
        fscanf(file, "%d %d", &index, &data);
        insert_node_at(lst, index, data);
        break;
    case DELETE_AT:
        fscanf(file, "%d", &index);
        delete_node_at(lst, index);
        break;
    case SEARCH_LIST:
        fscanf(file, "%d", &element);
        int ind = search_list(lst, element);
        print_index(ind);
        break;
    case REVERSE_LIST:
        reverse_list(lst);
        break;
    case RESET_LIST:
        reset_list(lst);
        break;
    case LIST_LEN:;
        int length = list_len(lst);
        print_length(length);
        break;
    case MAP:
        fscanf(file, "%d", &index);
        map(lst, func_list[index]);
        break;
    }
}

// Prints out the sum
void print_sum(long sum)
{
    printf("%ld\n", sum);
}

// Prints out length of list
void print_length(int length)
{
    printf("%d\n", length);
}

// Print index
void print_index(int index)
{
    if (index == -2)
        printf("{}\n");
    else
        printf("{%d}\n", index);
}
