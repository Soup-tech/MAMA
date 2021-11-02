#include "mm.h"
#include "term/utils.h"
#include <include/core/serial.h>

/// Start address of the heap
u32int start_addr;

mcb_queue_s allocated;
mcb_queue_s free;

/// Allocated Memory Control List
mcb_queue_s * amcb = &allocated;

/// Free Memory Control List
mcb_queue_s * fmcb = &free;

/// Full heap size
int fullHeapSize;

int initHeap(char * p) {
	skip_ws(&p);
	// Syntax for the cmd is: initheap [size]
	serial_println("");
	serial_println("DEBUG: initHeap");
	// Convert parameter from char * to int
	int size = atoi(p);
	int fullHeapSize = size + sizeof(cmcb_s);
	
	// Allocate to the heap
	start_addr = kmalloc(fullHeapSize);

	// Organize the heap. Both are of type FREE
	// ** CMCB at the top of the heap 		**
	// ** LMCB at the bottom of the heap 	**

	// CMCB at the top of the heap w/ all the information
	cmcb_s * head = (cmcb_s *) start_addr;
	head->type = FREE;
	head->addr = start_addr + sizeof(cmcb_s);
	head->size = (u32int) fullHeapSize - sizeof(cmcb_s);
	strcpy(head->name,"Initial CMCB");
	head->next = NULL;
	head->prev = NULL;

	printf("Heads Address: %i\n",(int) head->addr);
	printf("Heads Size: %i\n", (int) head->size);

	// Initialize free and allocated lists
	fmcb->mcbq_head = head;
	fmcb->mcb_queue_type = FREE;

	amcb->mcbq_head = NULL;
	amcb->mcb_queue_type = ALLOCATED;

	serial_println("");
	return 0;
}

int allocateMemory(char * size) {
	skip_ws(&size);
	serial_println("");
	serial_println("DEBUG: allocateMemory");
	// Calculate required size for allocated mcb
	u32int required = (u32int) (atoi(size) + sizeof(cmcb_s));
	u32int ref_size;

	// Is fmcb list empty?
	if (fmcb->mcbq_head == NULL) {
		serial_println("Error: Free Memory Control List is empty.");
		return -1;
	}

	// Iterate through the free memory until a block 
	// with enough space is found
	cmcb_s * queue = fmcb->mcbq_head;
	while (queue != NULL) {
		if (queue->size >= required) {
			ref_size = queue->size;
			break;
		}
		queue = queue->next;
	}

	printf("Available Memory: %i\n",queue->size);
	printf("Queue's Address: %i\n", queue->addr);

	// If no block with enough space is found, throw error
	if (queue == NULL) {
		serial_println("Error: No free memory available");
		return -1;
	}

	// Allocate memory
	// 1. Remove mcb that has enough space from the free list
	// 2. Allocate memory / Insert into allocated list
	// 3. Assign free cmcb in the next available free area
	
	// 1. Remove mcb with enough space
	removeFMCB(queue);
	
	// 2. Allocate memory for the mcb
	cmcb_s * newAMCB = queue;
	newAMCB->type = ALLOCATED;
	newAMCB->addr = (u32int) queue->addr;
	newAMCB->size = (u32int) required;
	newAMCB->next = NULL;
	newAMCB->prev = NULL;

	printf("New AMCB addr: %i\n", newAMCB->addr);
	printf("New AMCB Size: %i\n", newAMCB->size);
	insertAMCB(newAMCB);
	printf("AMCB addr after insert: %i\n", newAMCB->addr);
	printf("AMCB size after insert: %i\n", newAMCB->size);
	serial_println("");

	// 3. Assign free cmcb in the next available free area
	cmcb_s * newFMCB = (cmcb_s *) newAMCB + required;
	newFMCB->type = FREE;
	newFMCB->addr = (u32int) newAMCB->addr + required + sizeof(cmcb_s); // 204 offset
	newFMCB->size = (u32int) ref_size - (required + sizeof(cmcb_s)); // 796
	newFMCB->next = NULL;
	newFMCB->prev = NULL;

	printf("New FMCB addr: %i\n", newFMCB->addr);
	printf("New FMCB size: %i\n", newFMCB->size);
	serial_println("");
	printf("AMCB addr before insert: %i\n", newAMCB->addr);
	printf("AMCB size before insert: %i\n", newAMCB->size);
	insertFMCB(newFMCB);

	printf("FMCB addr after insert: %i\n", newFMCB->addr);
	printf("FMCB size after insert: %i\n", newFMCB->size);

	serial_println("");

	printf("FMCB head addr: %i\n", fmcb->mcbq_head->addr);
	printf("AMCB head addr: %i\n", amcb->mcbq_head->addr);

	serial_println("");

	printf("FMCB head size: %i\n", fmcb->mcbq_head->size);
	printf("AMCB head size: %i\n", amcb->mcbq_head->size);

	serial_println("");
	return 0;
}

void removeFMCB(cmcb_s * cmcb) {
	serial_println("");
	serial_println("DEBUG: removeFMCB");

	// Only free cmcb in the list (only head)
	if ((cmcb->prev == NULL) && (cmcb->next == NULL)) {
		serial_println("Removing only head");
		fmcb->mcbq_head = NULL;

		serial_println("");
		return;
	}
	// There is a free cmcb after, but not before (is head w/ members)
	else if ((cmcb->prev == NULL) && (cmcb->next != NULL)) {
		serial_println("Head with members");
		// Assign new head
		fmcb->mcbq_head = cmcb->next;

		// Terminate current head
		fmcb->mcbq_head->prev = NULL;

		serial_println("");
		return;
	}
	// There is a free cmcb before and after (somewhere in the middle of the list)
	else if ((cmcb->prev != NULL) && (cmcb->next != NULL)) {
		serial_println("Somewhere in the middle");
		// Save reference to previous cmcb
		cmcb_s * pCmcb = cmcb->prev;

		// Save reference to next cmcb
		cmcb_s * nCmcb = cmcb->next;

		// Link previous and next
		pCmcb->next = nCmcb;
		nCmcb->prev = pCmcb;

		serial_println("");
		return;
	}
	// There is a previous cmcb but no next free cmcb (tail)
	else if ((cmcb->prev != NULL) && (cmcb->next == NULL)) {
		serial_println("At the tail");
		// Reference previous cmcb
		cmcb_s * pCmcb = cmcb->prev;

		// Terminate the tail
		pCmcb->next = NULL;

		serial_println("");
		return;
	}

	serial_println("");
}

void insertAMCB(cmcb_s * mcb) {
	serial_println("");
	serial_println("DEBUG: insertAMCB");

	// No allocated mcb's
	if (amcb->mcbq_head == NULL) {
		serial_println("AMCB is empty. Creating head");
		amcb->mcbq_head = mcb;

		serial_println("");
		return;
	}
	
	// Organize by increasing address
	cmcb_s * queue = amcb->mcbq_head;
	while ((mcb->addr > queue->addr) && (queue != NULL)) {
		queue = queue->next;
	}

	// Address is smaller than head
	if ((queue->next != NULL) && (queue->prev == NULL)) {
		serial_println("New AMCB head");
		// Assign new head
		amcb->mcbq_head = mcb;

		// Link everything up
		queue->prev = mcb;
		mcb->prev = NULL;
		mcb->next = queue;

		serial_println("");
		return;
	}
	// Address is somewhere in the middle
	else if ((queue->prev != NULL) && (queue->next != NULL)) {
		serial_println("Linking new mcb somewhere in middle");
		// Save reference to previous cmcb
		cmcb_s * pCmcb = queue->prev;

		// Save reference to next cmcb
		cmcb_s * nCmcb = queue->next;

		// Link everything up
		pCmcb->next = mcb;
		nCmcb->prev = mcb;
		mcb->next = nCmcb;
		mcb->prev = pCmcb;

		serial_println("");
		return;
	}
	// Address is highest (tail)
	else if (queue == NULL) {
		serial_println("New Tail");
		// Reference to previous mcb
		cmcb_s * pCmcb = queue->prev;

		// Link new tail
		pCmcb->next = mcb;
		mcb->prev = pCmcb;

		serial_println("");
		return;
	}

	serial_println("");
}

void insertFMCB(cmcb_s * mcb) {
	serial_println("");
	serial_println("DEBUG: insertFMCB");

	// Is there a head to the fmcb list?
	if (fmcb->mcbq_head == NULL) {
		serial_println("Create new free head");
		fmcb->mcbq_head = mcb;
		// serial_println(fmcb->mcbq_head->name);

		serial_println("");
		return;
	}

	// Address in ascending order
	cmcb_s * queue = fmcb->mcbq_head;
	while ((mcb->addr > queue->addr) && (queue != NULL)) {
		queue = queue->next;
	}

	// MCB address is smaller than heads address
	if ((queue->prev == NULL) && (queue->next != NULL)) {
		serial_println("New FMCB head");
		// Assign new head
		fmcb->mcbq_head = mcb;

		// Link mcb's
		mcb->next = queue;
		queue->prev = mcb;

		serial_println("");
		return;
	}
	// Address is middling
	else if ((queue->prev != NULL) && (queue->next != NULL)) {
		serial_println("FMCB is middling");
		// Refer previous node
		cmcb_s * pCmcb = queue->prev;

		// Refer next node
		cmcb_s * nCmcb = queue->next;

		// Link everything up
		pCmcb->next = mcb;
		nCmcb->prev = mcb;
		mcb->next = nCmcb;
		mcb->prev = pCmcb;

		serial_println("");
		return;		
	}
	// Largest address (tail)
	else if ((queue->prev != NULL) && (queue->next == NULL)) {
		serial_println("New FMCB is at tail");

		// Refer old tail
		cmcb_s * pCmcb = queue->prev;

		// Assign new tail
		pCmcb->next = mcb;
		mcb->prev = pCmcb;

		serial_println("");
		return;
	}

	serial_println("");
}

int showAllocated(char *discard) {
	(void)discard;

	cmcb_s *block = amcb->mcbq_head;
	if(block == NULL) {
		printf("No allocated memory found\n");
		return 0;
	}
	while(block != NULL) {
		printf("Block %s - ", block->name);
		if(block->type == ALLOCATED)
			display_fg_color(RED);
		else
			display_fg_color(GREEN);
		printf(block->type == ALLOCATED ? "ALLOCATED" : "FREE");
		display_reset();
		printf(" - base addr: %i, size: %i bytes\n", block->addr, block->size);
		block = block->next;
	}
	return 0;
}

int isEmpty() {
	if(amcb->mcbq_head == NULL){
		writelnMessage("Memory is empty");
		return 0;
	}
	else {
		writelnMessage("Memory is not empty");
		return -1;
	}	
} 
