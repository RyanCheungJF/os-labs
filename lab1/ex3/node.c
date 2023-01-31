/*************************************
 * Lab 1 Exercise 2
 * Name: Ryan Cheung Jing Feng
 * Student No: A0217587R
 * Lab Group: 18
 *************************************/

#include "node.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Add in your implementation below to the respective functions
// Feel free to add any headers you deem fit (although you do not need to)

// Traverses list and returns the sum of the data values
// of every node in the list.
long sum_list(list *lst)
{
    node *curr = lst->head;
    int sum = 0;

    while (curr != NULL)
    {
        sum += curr->data;
        curr = curr->next;
    }
    return sum;
}

// Traverses list and returns the number of data values in the list.
int list_len(list *lst)
{
    node *curr = lst->head;
    int counter = 0;

    while (curr != NULL)
    {
        curr = curr->next;
        counter++;
    }
    return counter;
}

// Inserts a new node with data value at index (counting from head
// starting at 0).
// Note: index is guaranteed to be valid.
void insert_node_at(list *lst, int index, int data)
{
    // typedef just means you can call 'struct NODE' or just 'node'
    node *newNode;
    // reserving memory for new node
    newNode = (node *)malloc(sizeof(node));
    newNode->data = data;

    // if list is empty
    if (lst->head == NULL || index == 0)
    {
        newNode->next = lst->head;
        lst->head = newNode;
    }
    else
    {
        node *curr = lst->head;
        for (int i = 0; i < index - 1; i++)
        {
            curr = curr->next;
        }
        node *temp = curr->next;
        curr->next = newNode;
        newNode->next = temp;
    }
}

// Deletes node at index (counting from head starting from 0).
// Note: index is guarenteed to be valid.

void delete_node_at(list *lst, int index)
{
    node *curr = lst->head;
    if (index == 0)
    {
        lst->head = curr->next;
    }
    else
    {
        for (int i = 0; i < index - 1; i++)
        {
            curr = curr->next;
        }
        node *temp = curr;
        curr = curr->next;
        temp->next = curr->next;
    }
    free(curr);
}

// Search list by the given element.
// If element not present, return -1 else return the index. If lst is empty return -2.

int search_list(list *lst, int element)
{
    // empty list
    if (lst->head == NULL)
    {
        return -2;
    }

    node *curr = lst->head;
    int index = 0;
    while (curr != NULL)
    {
        if (curr->data == element)
        {
            return index;
        }
        index++;
        curr = curr->next;
    }
    return -1;
}

// Reverses the list with the last node becoming the first node.
void reverse_list(list *lst)
{
    node *prev = NULL;
    node *curr = lst->head;
    node *nxt = NULL;

    while (curr != NULL)
    {
        // store after curr
        nxt = curr->next;
        // reverse order
        curr->next = prev;
        // swap pointers
        prev = curr;
        curr = nxt;
    }
    // reset head
    lst->head = prev;
}

// Resets list to an empty state (no nodes) and frees
// any allocated memory in the process
void reset_list(list *lst)
{
    node *curr = lst->head;
    while (curr != NULL)
    {
        node *temp = curr->next;
        free(curr);
        curr = temp;
    }
    lst->head = NULL;
}

// Traverses list and applies func on data values of
// all elements in the list.
void map(list *lst, int (*func)(int))
{
    node *curr = lst->head;
    while (curr != NULL)
    {
        // applies function pointer to data
        curr->data = func(curr->data);
        curr = curr->next;
    }
}
