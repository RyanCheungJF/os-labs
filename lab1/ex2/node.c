/*************************************
 * Lab 1 Exercise 2
 * Name: Ryan Cheung Jing Feng
 * Student No: A0217587R
 * Lab Group: 18
 *************************************/

#include <stdio.h>
#include <stdlib.h>
#include "node.h"

// Add in your implementation below to the respective functions
// Feel free to add any headers you deem fit

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
    for (int i = 0; i < index - 1; i++)
    {
        curr = curr->next;
    }
    node *temp = curr;
    curr = curr->next;
    temp->next = curr->next;
    free(curr);
}

// Search list by the given element.
// If element not present, return -1 else return the index. If lst is empty return -2.
// Printing of the index is already handled in ex2.c
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
        nxt = curr->next;
        // reverse order
        curr->next = prev;
        nxt->next = curr;
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