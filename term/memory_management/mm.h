#ifndef MM_H
#define MM_H 

/********************************************/
/**************** Structures ****************/
/********************************************/

/// Indicates the type of CMCB
typedef enum {
    /// Allocated CMCB
    ALLOCATED,

    /// Freed CMCB
    FREE
} mcb_state_e;

/// Complete Memory Control Block (CMBC)
typedef struct cmcb_s {
    /// The type of the CMCB    
    mcb_state_e type;

    /// Beginning address of the CMCB
    u32int addr;

    /// Size of the CMCB
    u32int size;

    /// Name of CMCB
    char name[32];

    /// Next CMCB
    struct cmcb_s * next;

    /// Previous CMCB
    struct cmcb_s * prev;
} cmcb_s;

/// Limited Memory Control Block
typedef struct lmcb_s {
    /// Type of the LMCB
    mcb_state_e type;

    /// Size of the LMCB
    u32int size;
} lmcb_s;

/// "Master" controller of the PCB queue
typedef struct mcb_queue_s {
    /// Number of PCB's currently in the queue
    int mcb_count;     

    /// Head of the PCB queue
    cmcb_s * mcbq_head; 

    /// Tail of the PCB queue
    cmcb_s * mcbq_tail; 

    /// Queue order of the Master controller
    mcb_state_e mcb_queue_type;
} mcb_queue_s;

/********************************************/
/************ Function Headers **************/
/********************************************/

/**
 * Allocate all memory available for the MPX
 * 
 * Allocates memory for both CMBC and LMCB. This will
 * put a CMCB at the top of the heap and a LMCB at the bottom
 * of the heap, both of type free. This method also intializes 
 * the free and allocated lists
 * 
 * @param size Size that will be allocated for the heap in bytes 
 * 
 * @return Return 0 upon success, -1 otherwise
*/
int initHeap(char * p);

/**
 * Allocate additional memory from the heap.
 * 
 * Allocates additional memory from the heap in a
 * first-fit method. 
 * 
 * @params size Amount of bytes to be allocated from the heap
 * 
 * @return Returns 0 upon success, -1 otherwise 
*/
int allocateMemory(char * size);

/**
 * Free a block of memory
 * 
 * Frees a particular block of memory that was
 * previously allocated. Searches for the block of memory,
 * removes it from the allocated list and places it into the
 * free list. If there are any adjacent blocks then merge.
 * 
 * @params addr Address of the block that will be free
 * 
 * @return Returns 0 upon success, -1 otherwise
*/
int freeMemory(char * addr);

/**
 * Shows addresses and block size of all blocks in
 * allocated list
 * 
 * Traverses the allocated list and shows the addresses 
 * and the size of the block. Shown in the order of address. 
*/
void showAllocated();

/**
 * Shows the addresses and block size of all block in 
 * free list
 * 
 * Traverses the free list and shows the addresses and the 
 * size of the block. Shown in the order of address.
*/
void showFree();

/**
 * Does the heap only contain free memory.
 * 
 * Tells whether the heap contains only free memory (True)
 * or not (false).
 * 
 * @return Returns 1 (True) if heap contains only free memory, Return 0 (False) 
 * 		   if there is something within the heap. 
*/
int isEmpty();

#endif
