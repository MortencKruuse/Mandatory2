#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "mymem.h"
#include <time.h>
#include <stdbool.h>


/* The main structure for implementing memory allocation.
 * You may change this to fit your implementation.
 */

struct memoryList
{
  // doubly-linked list
  struct memoryList *last;
  struct memoryList *next;

  int size;            // How many bytes in this block?
  char alloc;          // 1 if this block is allocated,
                       // 0 if this block is free.
  void *ptr;           // location of block in memory pool.
};

strategies myStrategy = NotSet;    // Current strategy


size_t mySize;
void *myMemory = NULL;

static struct memoryList *head;
static struct memoryList *next;


/* initmem must be called prior to mymalloc and myfree.

   initmem may be called more than once in a given exeuction;
   when this occurs, all memory you previously malloc'ed  *must* be freed,
   including any existing bookkeeping data.

   strategy must be one of the following:
		- "best" (best-fit)
		- "worst" (worst-fit)
		- "first" (first-fit)
		- "next" (next-fit)
   sz specifies the number of bytes that will be available, in total, for all mymalloc requests.
*/

void initmem(strategies strategy, size_t sz) {
    myStrategy = strategy;
    /* all implementations will need an actual block of memory to use */
    mySize = sz;

    struct memoryList *trav;
    if (myMemory != NULL) free(myMemory); /* in case this is not the first time initmem2 is called */
    /* TODO: release any other memory you were using for bookkeeping when doing a re-initialization! Maybe this works? */
    if (head != NULL) {
        for (trav = head; trav->next != NULL; trav = trav->next) {
            free(trav->last);
            free(trav);
        }
    }
    free(head);



    //Allocating a block of memory with size sz
    myMemory = malloc(sz);
    //Allocating a block of memory for our struct, so it can keep track of the other block of memory we allocated
    head = malloc(sizeof(struct memoryList));
    //Setting the size tracker of the struct to be the size of our allocated memory block
    head->size = sz;
    //Setting the amount of allocated memory in our memory block to be equal 0 since we've yet to deposit any information in it
    head->alloc = 0;
    //Setting the pointer for our doubly linked list to point to our block of memory that we've allocated
    head->ptr = myMemory;
    /* TODO: Initialize memory management structure. Done */
    head->last = head;
    head->next = head;


}

/* Allocate a block of memory with the requested size.
 *  If the requested block is not available, mymalloc returns NULL.
 *  Otherwise, it returns a pointer to the newly allocated block.
 *  Restriction: requested >= 1 
 */

void *mymalloc(size_t requested) {
    assert((int) myStrategy > 0);
    struct memoryList *temp = head;
    bool found = false;

    switch (myStrategy) {
        case NotSet:
            return NULL;
        case First:
            do {
                if (temp->size >= requested && temp->alloc == 0) {
                    found = true;
                    break;
                }
                temp = temp->next;
            } while (temp != head);
            break;

        case Best:
            return NULL;
        case Worst:
            return NULL;
        case Next:
            return NULL;
    }

    /*
     * if statement checks whether the size of the temp block is bigger than the requested size,
     * and if so should be allocated to a new block.
     * The if statement updates the pointers for both blocks and sizes
     */
    if(found) {
        if (temp->size > requested) {
            struct memoryList *leftovers = malloc(sizeof(struct memoryList));
            leftovers->next = temp->next;
            leftovers->next->last = leftovers;
            leftovers->last = temp;
            temp->next = leftovers;

            leftovers->size = temp->size - requested;
            temp->size = requested;

            leftovers->alloc = 0;

            leftovers->ptr = temp->ptr + requested;

            //for later next* should be updated here
        } else { next = temp->next; }

        temp->alloc = 1;
        return temp->ptr;
    } else{
        printf("No memory block of the %zu size found\n",requested);
        return  NULL;}

}


/* Frees a block of memory previously allocated by mymalloc. */
void myfree(void *block) {
    return;
}

/****** Memory status/property functions ******
 * Implement these functions.
 * Note that when refered to "memory" here, it is meant that the 
 * memory pool this module manages via initmem/mymalloc/myfree. 
 */

/* Get the number of contiguous areas of free space in memory. */
int mem_holes()
{
    int i = 0;
    struct memoryList* current = head;
    do{
        if (current->last->alloc == 0 && current->next->alloc != 0){
            i++;
        }
    } while (current->next != head);
    return i;}

/* Get the number of bytes allocated */
int mem_allocated() {
    int i = 0;
    struct memoryList *current = head;
    do {
        if (current->alloc == 1) {
            i += current->size;
        }
    } while (current->next != head);
    return i;
}

/* Number of non-allocated bytes */
int mem_free() {
    int i = 0;
    struct memoryList *current = head;
    do {
        if (current->alloc == 0) {
            i += current->size;
        }
    } while (current->next != head);
    return i;
}

/* Number of bytes in the largest contiguous area of unallocated memory */
int mem_largest_free() {
    int i = 0;
    struct memoryList *current = head;
    do {
        if (current->alloc != 0 & i < current->alloc) {
            i = current->size;
        }
    } while (current->next != head);
    return i;
}

/* Number of free blocks smaller than or equal to "size" bytes. */
int mem_small_free(int size) {
    int i = 0;
    struct memoryList *current = head;
    do {
        if (current->alloc != 0 && current->size <= size) {
            i++;
        }
    } while (current->next != head);
    return i;
}

char mem_is_alloc(void *ptr) {
    return 0;
}

/* 
 * Feel free to use these functions, but do not modify them.  
 * The test code uses them, but you may find them useful.
 */


//Returns a pointer to the memory pool.
void *mem_pool() {
    return myMemory;
}

// Returns the total number of bytes in the memory pool. */
int mem_total() {
    return mySize;
}


// Get string name for a strategy. 
char *strategy_name(strategies strategy) {
    switch (strategy) {
        case Best:
            return "best";
        case Worst:
            return "worst";
        case First:
            return "first";
        case Next:
            return "next";
        default:
            return "unknown";
    }
}

// Get strategy from name.
strategies strategyFromString(char *strategy) {
    if (!strcmp(strategy, "best")) {
        return Best;
    } else if (!strcmp(strategy, "worst")) {
        return Worst;
    } else if (!strcmp(strategy, "first")) {
        return First;
    } else if (!strcmp(strategy, "next")) {
        return Next;
    } else {
        return 0;
    }
}


/* 
 * These functions are for you to modify however you see fit.  These will not
 * be used in tests, but you may find them useful for debugging.
 */

/* Use this function to print out the current contents of memory. */
void print_memory() {
    int i = 0;
    struct memoryList *current = head;
    do {
        printf("Block=%i\\;"
               "\n\tsize=%d"
               "\n\talloc=%c"
               "\n",i,current->size,current->alloc ? 1 : 0);
        i++;
    } while (current->next != head);
	return;
}

/* Use this function to track memory allocation performance.  
 * This function does not depend on your implementation, 
 * but on the functions you wrote above.
 */
void print_memory_status() {
    printf("%d out of %d bytes allocated.\n", mem_allocated(), mem_total());
    printf("%d bytes are free in %d holes; maximum allocatable block is %d bytes.\n", mem_free(), mem_holes(),
           mem_largest_free());
    printf("Average hole size is %f.\n\n", ((float) mem_free()) / mem_holes());
}

/* Use this function to see what happens when your malloc and free
 * implementations are called.  Run "mem -try <args>" to call this function.
 * We have given you a simple example to start.
 */
void try_mymem(int argc, char **argv) {
    strategies strat;
    void *a, *b, *c, *d, *e;
    if (argc > 1)
        strat = strategyFromString(argv[1]);
    else
        strat = First;


    /* A simple example.
       Each algorithm should produce a different layout. */

    initmem(strat, 500);

    a = mymalloc(100);
    b = mymalloc(100);
    c = mymalloc(100);
    myfree(b);
    d = mymalloc(50);
    myfree(a);
    e = mymalloc(25);

    print_memory();
    print_memory_status();

}
