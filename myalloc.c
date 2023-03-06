#include <stdio.h>
#include <sys/mman.h>

#define ALIGNMENT 16
#define GET_PAD(x) ((ALIGNMENT - 1) - (((x) - 1) & (ALIGNMENT - 1)))
#define PADDED_SIZE(x) ((x) + GET_PAD(x))
#define PTR_OFFSET(p, offset) ((void*)((char *)(p) + (offset)))

struct block {
    struct block *next;
    int size;     // Bytes
    int in_use;   // Boolean
};

void *myalloc(int size);
void split_block(struct block *n, int padded_data_size);
void myfree(void *p);
void print_data(void);

struct block *head = NULL;
int padded_block_size = PADDED_SIZE(sizeof(struct block));



int main(void) {}



void *myalloc(int size) {
    if (head == NULL) {
            head = mmap(NULL, 1024, PROT_READ|PROT_WRITE, MAP_ANON|MAP_PRIVATE, -1, 0);
            head->next = NULL;
            head->size = 1024 - PADDED_SIZE(sizeof(struct block));
            head->in_use = 0;
    }
    struct block *n = head;
    int padded_data_size = PADDED_SIZE(size);

    while (n != NULL) {
        if ( (n->size >= padded_data_size) && (!n->in_use) ) {
            int required_space = padded_data_size + padded_block_size + 16;

            if (n->size >= required_space){
                split_block(n, padded_data_size);
            }
            n->in_use = 1;
            return PTR_OFFSET(n, padded_block_size);
        }
        n = n->next;
    }
    return NULL; 
}


void split_block(struct block *n, int padded_data_size) {
    int leftover_size = n->size - padded_data_size;
    struct block *m = PTR_OFFSET(n, padded_block_size + padded_data_size);

    m->next = n->next;
    n->next = m;
    m->size = leftover_size - padded_block_size;
    n->size = padded_data_size;
    m->in_use = 0;
}


void myfree(void *p) {
    struct block *n = p - padded_block_size;
    n->in_use = 0;

    struct block *cur = head;
    while (cur->next != NULL) {
        if (cur->in_use == 0 && (cur->next)->in_use == 0) {
            cur->size += padded_block_size + (cur->next)->size;
            cur->next = (cur->next)->next;
        }
        else {
            cur = cur->next;
        }
    }
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