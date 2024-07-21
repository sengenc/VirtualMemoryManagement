#include "vmm.h"
#include "output_utility.h"

#include <stdio.h>
#include <stdlib.h>

#define PAGE_TABLE_ENTRIES 256
#define PAGE_SIZE 256
#define TLB_ENTRIES 16
#define FRAME_SIZE 256
#define FRAMES 64
#define PHYSICAL_MEM_SIZE (FRAMES * FRAME_SIZE)

#define PAGE_NUMBER( FROM_NUMBER ) ( (FROM_NUMBER&0x0000ff00)>>8 )
#define OFFSET( FROM_NUMBER ) (FROM_NUMBER&0x000000ff)

FILE *backing_store_file_ptr;
unsigned char *backing_store;

struct Page {
    unsigned char values[255];
    int frame_index;
    struct Page *bevor;
    struct Page *after;
    bool tlb;
    bool pagetable;
};

typedef struct Page *page;

struct Linked_number {
    int logical_address;
    int number;
    int index;
    unsigned int page_number;
    unsigned int offset;
    struct Linked_number *next;
    page page;
};

typedef struct Linked_number *linked_number;

page *page_table;

int read_addresses = 0;
int addresses_index = 0;
bool all_numbers_read = false;

int read_frames = 0;


struct Linked_number *current_number_ptr = NULL;
    

/**
 * Initialized a statistics object to zero.
 * @param stats A pointer to an uninitialized statistics object.
 */
static void statistics_initialize(Statistics *stats) {
    stats->tlb_hits = 0;
    stats->pagetable_hits = 0;
    stats->total_memory_accesses = 0;
}

void init_page_table() {
    page_table = (page *)malloc(sizeof(page)*PAGE_TABLE_ENTRIES);
    printf("memory allocated!\n");
    for (int i = 0; i < PAGE_TABLE_ENTRIES-1; i++) {
        page p = (page)malloc(sizeof(struct Page));
        p->frame_index = -1;
        page_table[i] = p;
    }
    printf("pagetable initialized!\n");
}

void read_backing_store(FILE *fd_backing) {
    backing_store = (unsigned char *)malloc(65536*sizeof(char));
    int i = 0;
    fread(backing_store, 65536, 1, fd_backing);
    /**
    for (int byte = 0; byte < 65536; byte++)
    {
        if((i%16) == 0) {
            printf("0x%04x: ", i);
            for (int j = 0; j < 16; j++)
            {
                printf("%02x ", backing_store[i+j]);
            }
            printf("\n");
        }
        i++;
    }
    */
}

linked_number new_link() {
    // create new linked number element and init next pointer for later use
    linked_number next = (linked_number)malloc(sizeof(struct Linked_number));
    next->next = NULL;
    return next;
}

linked_number next_number() {
    if(current_number_ptr == NULL) {
        current_number_ptr =  new_link();
        return current_number_ptr;
    } else {
        current_number_ptr->next = new_link();
        return current_number_ptr->next;
    }
}

void read_next_number(FILE *fd_addresses, linked_number number) {
    char buffer;
    int count = 0;
    while (fread(&buffer, 1, 1, fd_addresses)) {
        ++count;
        ++addresses_index;
        // if buffer is newline 
        if(buffer == 10) {
            // allocate memory for bytewise number
            char *to_read = (char *)malloc(count*sizeof(char));

            // move filedescriptor to start of number and read number byte by byte
            fseek(fd_addresses, (addresses_index-count), SEEK_SET);
            for (int i = 0; i < count; i++) {
                fread(&to_read[i], 1, 1, fd_addresses);
            }
            number->logical_address = atoi(to_read);
            number->page_number = PAGE_NUMBER(number->logical_address);
            number->offset = OFFSET(number->logical_address);
            number->index = read_addresses++;

            printf("number: %d, page: %d, offset: %d\n", number->logical_address, number->page_number, number->offset);

            // free temp buffer for bytes
            free(to_read);
            
            // debug feature just using 10 values
            if(read_addresses == 10) all_numbers_read = true;
            
            return;
        }
    }
    all_numbers_read = true;
}

page get_page_from_tlb(int page_index) {
    return NULL;
}

page get_page_from_pagetable(int page_index) {
    return NULL;
}

page load_page_from_backingstore() {
    page toread = (page)malloc(sizeof(struct Page));
    fread(toread->values, PAGE_SIZE, 1, backing_store_file_ptr);
    for (int i = 0; i < PAGE_SIZE; i++)
    {
        //printf("byte: %x\n", toread->values[i]);
    }
    toread->frame_index = read_frames++;
    return toread;
}

page get_page_by_index(int page_index) {
    page to_return = NULL;
    
    //checking tlb
    to_return = get_page_from_tlb(page_index);
    if(to_return != NULL) {
        to_return->pagetable = false;
        to_return->tlb = true;
        return to_return;
    }

    //checking pagetable
    to_return = get_page_from_pagetable(page_index);
    if(to_return != NULL) {
        to_return->pagetable = false;
        to_return->tlb = true;
        return to_return;
    }
    //load page from backingstore
    do {
        to_return = load_page_from_backingstore();
        
        if(to_return == NULL) return NULL;
    } while (to_return->frame_index < page_index);
    printf("read page: %d\n", page_index);
    return to_return;
}

void print_number(linked_number number_to_print) {
    
    number_to_print->page = get_page_by_index(number_to_print->page_number);
    printf("Virtual: %05d, ", number_to_print->logical_address);
    printf("Physical: %5d, ", ((256* number_to_print->index) + number_to_print->offset));
    number_to_print->number = number_to_print->page->values[number_to_print->offset];
    printf("Value: %x, ", number_to_print->number);
    printf("TLB hit: %s,", number_to_print->page->tlb ? "true" : "false");
    printf(" PT hit: %s\n", number_to_print->page->pagetable ? "true" : "false");
}

Statistics simulate_virtual_memory_accesses(FILE *fd_addresses, FILE *fd_backing) {
    // Initialize statistics
    Statistics stats;
    // TODO: Implement your solution

    init_page_table();
    read_backing_store(fd_backing);

    backing_store_file_ptr = fd_backing;
    
    printf("\nstart printing: \n");
    current_number_ptr = next_number();
    while (!all_numbers_read)
    {
        read_next_number(fd_addresses, current_number_ptr);
        // if current_number is loaded from memory
        print_number(current_number_ptr);
        // get next number loaded
        current_number_ptr = next_number();
    }

    return stats;
}
















linked_number get_number_by_index(linked_number list, int i);

linked_number get_number_by_index(linked_number list, int i) {
    linked_number n = list;
    if(n->index == i) {
        return n;
    }else{
        return get_number_by_index(n->next, i);
    }
}