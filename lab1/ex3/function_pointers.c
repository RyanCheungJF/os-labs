/*************************************
 * Lab 1 Exercise 3
 * Name: Ryan Cheung Jing Feng
 * Student No: A0217587R
 * Lab Group: 18
 *************************************/

#include "functions.h"

// Initialize the func_list array here!
int (*func_list[5])(int);

// You can also use this function to help you with
// the initialization. This will be called in ex3.c.
void update_functions()
{
    func_list[0] = add_one;
    func_list[1] = add_two;
    func_list[2] = multiply_five;
    func_list[3] = square;
    func_list[4] = cube;
}
