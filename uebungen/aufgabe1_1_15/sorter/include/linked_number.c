#include "linked_number.h"
#include <stdlib.h>
#include <stdio.h>

struct linked_number_t
{
    int value;
    linked_number_t *last_number;
    linked_number_t *next_number;
};

/* Function to insert a node at the beginning of a linked list */
void insertAtTheBegin(linked_number_t **start_ref, int value) 
{ 
    linked_number_t *ptr1 = (linked_number_t*)malloc(sizeof(linked_number_t)); 
    ptr1->value = value; 
    ptr1->next_number = *start_ref; 
    *start_ref = ptr1; 
} 
  
/* Function to print nodes in a given linked list */
void printList(linked_number_t *start) 
{ 
    linked_number_t *temp = start;  
    while (temp!=NULL) 
    { 
        printf("%d ", temp->value); 
        temp = temp->next_number; 
    } 
} 

/* Bubble sort the given linked list */
void bubbleSort(linked_number_t *start) 
{ 
    int swapped; 
    linked_number_t *ptr1; 
    linked_number_t *lptr = NULL;
  
    /* Checking for empty list */
    if (start == NULL) 
        return; 
  
    do
    { 
        swapped = 0; 
        ptr1 = start; 
  
        while (ptr1->next_number != lptr) 
        { 
            if (ptr1->value > ptr1->next_number->value) 
            { 
                swap(ptr1, ptr1->next_number); 
                swapped = 1; 
            } 
            ptr1 = ptr1->next_number; 
        } 
        lptr = ptr1; 
    } 
    while (swapped); 
} 
  
/* function to swap data of two nodes a and b*/
void swap(linked_number_t*a, linked_number_t *b) 
{ 
    int temp = a->value; 
    a->value = b->value; 
    b->value = temp; 
} 