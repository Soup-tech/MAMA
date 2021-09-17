
#include "pcb.h"

#include <modules/mpx_supt.h>
#include <include/string.h>
#include <lib/out.h>

/*
	We are going to have two queues for right now. One will be priority 
	and the other will be FIFO.

*/
pcb_queue_t * pcb_queues[2];


/********************************************************/
/****************** Backend stuff here ******************/
/********************************************************/

pcb_t * allocatePCB() {
	/* Initialize PCB */
	pcb_t *pcb = (pcb_t *) sys_alloc_mem(sizeof(pcb_t));

	if (pcb == NULL) {
		return NULL;
	}

	/* Beginning of the stack (BP) */
	pcb->pcb_stack_bottom = (unsigned char *) sys_alloc_mem(MAX_STACK_SIZE);
	if (pcb->pcb_stack_top == NULL) {
		return NULL;
	}

	/* End of the stack (SP) */
	pcb->pcb_stack_top = pcb->pcb_stack_bottom + MAX_STACK_SIZE;

	/* Zero out memory in the stack frame (SF) */
	memset(pcb, MAX_STACK_SIZE, '\0');

	return pcb;
}

int freePCB(pcb_t * pcb) {
	int free = sys_free_mem(pcb->pcb_stack_bottom);
	free = sys_free_mem(pcb);
	return free;
}

pcb_t * setupPCB(char * name, pc_t process_class, int priority) {

	/* PCB name can only be 32 bytes long too */
	if (strlen(name) > MAX_NAME_SIZE) {
		print("PCB name too long. Please use a shorter name\n",45);
		return NULL;
	} else if (findPCB(name) != NULL) { /* PCB name cannot already exist */
		print("PCB name already in use. Please use a different name\n",53);
		return NULL;
	}

	/* Can only have a priority level from 0 to 9 */
	if (priority > MAX_PRIORITY || priority < MIN_PRIORITY) {
		print("PCB priority out of range\n",26);
		return NULL;
	}

	/* Ensure process class is SYS_PROCESS or APPLICATION */
	if (process_class != SYS_PROCESS || process_class != APPLICATION) {
		print("Unknown process class\n",22);
		return NULL;
	}

	pcb_t *pcb = allocatePCB();
	
	if (pcb == NULL) {
		print("Something went wrong setting up PCB\n",35);
		return NULL;
	}

	strcpy(pcb->pcb_name, name);
	pcb->pcb_process_class = process_class;
	pcb->pcb_priority = priority;
	pcb->pcb_process_state = READY;
	
	return pcb;
}

pcb_t * findPCB(char * name) {
	/* Check for valid name */
	if (strlen(name) > MAX_NAME_SIZE || name == NULL) {
		// TODO Error message here
		return NULL;
	}

	// Iterate through each PCB queues
	// Iterate through selected PCB queue
	// If strcmp(name,pcb->pcb_name)
	// return pcb
	//
	// return NULL

	// temporary
	pcb_t * pcb = allocatePCB();
	return pcb;
	
}

void insertPCB(pcb_t * pcb) {

	if (pcb == NULL) {
		return;
	}

	

	return;
}

/********************************************************/
/*************** User Command stuff here ****************/
/********************************************************/


int createPCB(char * args) {
	// The format of createpcb is : pcb_name.pcb_process_class.pcb_priority
	// So like... first_pcb_block.APPLICATION.3

	char * name;
	char * token;
	int priority, params = 1;
	pc_t process_class;

	token = strtok(args,".");
	name = token;

	/** Parse the user input **/
	while (token != NULL) {
		token = strtok(NULL,".");

		switch (params) {
			case 1:
				process_class = (pc_t) token;
				break;
			case 2:
				priority = atoi(token);
				break;
			default:
				break;
		}

		params++;
	}

	/** Error Handling **/
	/* Check for correct number of parameters */
	if (params == 1) {
		print("Usage: createpcb [NAME.PROCESS_CLASS.PRIORITY]\n",47);
		return 0;
	}
	else if (params != 4) {
		print("Error: Wrong amount of parameters\n",27);
		return 0;
	}

	/* Check name for size and whether it exists in a PCB already */
	if (strlen(name) > MAX_NAME_SIZE || name == NULL) {
		print("Error: Name of the PCB is too long\n",35);
		return 0;
	} 
	else if (findPCB(name) != NULL) {
		print("Error: PCB with that name already exists\n",41);
		return 0;
	}

	/* Check whether priority is out of bounds */
	if (priority > MAX_PRIORITY || priority < MIN_PRIORITY) {
		print("Error: Specified priority is out of bounds\n",43);
		return 0;
	}

	/* Check whether this is a valid process class */
	if (process_class != SYS_PROCESS || process_class != APPLICATION) {
		print("Error: Specified process class does not exist\n",46);
		return 0;
	}
	

	/* Create the PCB */
	pcb_t *pcb = allocatePCB(name, process_class, priority);

	/* Insert into PCB queue */
	insertPCB(pcb);

	return 0;
}
