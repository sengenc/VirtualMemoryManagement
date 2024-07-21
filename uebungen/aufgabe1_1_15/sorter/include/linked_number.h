#ifndef LINKED_NUMBER_H
#define LINKED_NUMBER_H

typedef struct linked_number_t linked_number_t;

void insertAtTheBegin(linked_number_t **start_ref, int value);
void printList(linked_number_t *start);
void bubbleSort(linked_number_t *start);
void swap(linked_number_t *a, linked_number_t *b);

#endif