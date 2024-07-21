#include <stdio.h>
#include <stdlib.h>
#include "include/linked_number.h"

#define MAX_INPUT 100

int main(int argc, char *argv[]) {
    char *input = (char *)malloc(sizeof(char)*MAX_INPUT);
    linked_number_t *start = NULL;
    
    while (1)
    {   
        // scan numbers
        printf("Eingabe: ");
        fgets(input, MAX_INPUT, stdin);

        // sort for numbers
        for (int i = 0; i < MAX_INPUT; i++)
        {
            if(input[i] >= 0x30 && input[i] <= 0x39) {
                // place numbers in structs
                insertAtTheBegin(&start, atoi(&input[i]));    
            }
        }
        
        // sorting numbers
        bubbleSort(start);

        // printing output
        printf("Ausgabe: ");
        printList(start);
        printf("\n");
    }

}