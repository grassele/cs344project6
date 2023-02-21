#include <stdio.h>
#include <sys/mman.h>


// clang -Wall -Wextra -o myalloc myalloc.c


// macros
#define ALIGNMENT 16   // Must be power of 2
#define GET_PAD(x) ((ALIGNMENT - 1) - (((x) - 1) & (ALIGNMENT - 1)))
#define PADDED_SIZE(x) ((x) + GET_PAD(x))
#define PTR_OFFSET(p, offset) ((void*)((char *)(p) + (offset)))


// declarations
struct block {
    struct block *next;
    int size;     // Bytes
    int in_use;   // Boolean
};
void *myalloc(int size);
void print_data(void);
// void myfree(void *p);


// global variable
struct block *head = NULL;
// size_t available_space = 1024; // added



int main(void) {    
    
}



void *myalloc(int size) {
    if (head == NULL) {
            head = mmap(NULL, 1024, PROT_READ|PROT_WRITE, MAP_ANON|MAP_PRIVATE, -1, 0);
            head->next = NULL;
            head->size = 1024 - PADDED_SIZE(sizeof(struct block));
            // available_space = head->size; // added
            head->in_use = 0;
    }
    
    struct block *n = head;
    size_t padded_block_size = PADDED_SIZE(sizeof(struct block));
    while (n != NULL) {
        // struct block *prev = head; - for modification of "next" field
        if (n->size >= PADDED_SIZE(size) && !n->in_use) { // + block size?
            // n->size = PADDED_SIZE(size);
            n->in_use = 1;
            return PTR_OFFSET(n, padded_block_size);
        }
        n = n->next;
    }
    return NULL;
    
}

void print_data(void) {
    struct block *b = head;

    if (b == NULL) {
        printf("[empty]\n");
        return;
    }

    while (b != NULL) {
        // Uncomment the following line if you want to see the pointer values
        //printf("[%p:%d,%s]", b, b->size, b->in_use? "used": "free");
        printf("[%d,%s]", b->size, b->in_use? "used": "free");
        if (b->next != NULL) {
            printf(" -> ");
        }

        b = b->next;
    }

    printf("\n");
}

// void myfree(void *p);
