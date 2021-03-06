#ifndef CONTEXT_H
#define CONTEXT_H

#include "term/pcb/pcb.h"

/// Context of the currently operating process
typedef struct context {
	/// Segment registers
	u32int gs, fs, es, ds;

	/// General purpose registers
	u32int edi, esi, ebp, esp, ebx, edx, ecx, eax;

	// Other special registers
	u32int eip, cs, eflags;
} context;

/**
 * Causes commhand to yield
 * 
 * Forces commhand to yield to other processes.
 * If any processes are in the ready queue, they will
 * be executed.
 *  
*/
void yield();

/**
 * Loads r3 'processes'
 * 
 * Loads all r3 'processes' into memory in a suspended ready
 * state at any priority of the users choosing
 * 
 * @param p Empty parameter
 * 
 * @return Returns 0 upon success, 1 upon error **This may change
 * 
*/
int loadr3(char * p);

/**
 * Stores context on the stack
 * 
 * With a given pcb and method to run, the dispatcher
 * will store context registers onto the PCB stack.
 * 
 * @param pcb PCB where context is stored
 * @param func Method that is ran within the process 
*/
pcb_t * dispatcher(char * pcb, void (* func) (void)); 



#endif
