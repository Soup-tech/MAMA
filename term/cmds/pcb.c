#include <lib/out.h>
#include <term/args.h>
#include <term/pcb/pcb.h>

int cmd_pcb_resume(char *args) {
	(void)args;
	return 0;
}

int cmd_pcb_set_priority(char *args) {
	(void)args;
	return 0;
}

int cmd_pcb_show(char *args) {
	char *pcb_name;

	if(!next_unnamed_arg(parse_args(args), &pcb_name)) {
		println("Bad usage: PCB name not provided", 32);
		return 1;
	}

	pcb_t *pcb = findPCB(pcb_name);
	if(pcb == NULL) {
		println("Error: PCB not found", 20);
		return 1;
	}

	printf("Process %s - ", pcb->pcb_name);
	switch(pcb->pcb_process_state) {
		case RUNNING:
			display_fg_color(GREEN);
			printf("RUNNING\n");
			display_reset();
			break;
		case READY:
			display_fg_color(GREEN);
			printf("READY");
			display_reset();
			printf("(not suspended)\n");
			break;
		case BLOCKED:
			display_fg_color(YELLOW);
			printf("BLOCKED");
			display_reset();
			printf("(not suspended)\n");
			break;
		case SUSPENDED_READY:
			display_fg_color(RED);
			printf("READY");
			display_reset();
			printf("(");
			display_fg_color(RED);
			printf("SUSPENDED");
			display_reset();
			printf(")\n");
			break;
		case SUSPENDED_BLOCKED:
			display_fg_color(RED);
			printf("BLOCKED");
			display_reset();
			printf("(");
			display_fg_color(RED);
			printf("SUSPENDED");
			display_reset();
			printf(")\n");
			break;
	}
	
	printf("   Proc ID: %i\n", 0); // TODO: placeholder until/if process id is added
	printf("     Class: %s\n", pcb->pcb_process_class == 0 ? "SYS_PROCESS" : "APPLICATION");
	printf("  Priority: %i [%s]\n", pcb->pcb_priority, "PLACEHOLDER");

	return 0;
}

int cmd_pcb_show_ready(char *args) {
	(void)args;
	return 0;
}

int cmd_pcb_show_blocked(char *args) {
	(void)args;
	return 0;
}

int cmd_pcb_show_all(char *args) {
	(void)args;
	// TODO: also show the running proc
	cmd_pcb_show_ready(NULL);
	cmd_pcb_show_blocked(NULL);
	return 0;
}

int cmd_pcb_suspend(char *args) {
	char *pcb_name;

	if(!next_unnamed_arg(parse_args(args), &pcb_name)) {
		printf("Bad usage: PCB name not provided\n");
		return 1;
	}

	pcb_t *pcb = findPCB(pcb_name);
	if(pcb == NULL) {
		printf("Error: PCB not found\n");
		return 1;
	}

	removePCB(pcb);
	switch(pcb->pcb_process_state) {
		case RUNNING:
		case READY:
		case SUSPENDED_READY:
			pcb->pcb_process_state = SUSPENDED_READY;
			break;
		case BLOCKED:
		case SUSPENDED_BLOCKED:
			pcb->pcb_process_state = SUSPENDED_BLOCKED;
			break;
		default:
			return 1;
	}
	return insertPCB(pcb);
}