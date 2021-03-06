/*
  ----- kmain.c -----

  Description..: Kernel main. The first function called after
      the bootloader. Initialization of hardware, system
      structures, devices, and initial processes happens here.
      
      Initial Kernel -- by Forrest Desjardin, 2013, 
      Modifications by:    Andrew Duncan 2014,  John Jacko 2017
      				Ben Smith 2018, and Alex Wilson 2019
*/

#include <stdint.h>
#include <string.h>
#include <system.h>
#include <core/io.h>
#include <core/serial.h>
#include <core/tables.h>
#include <core/interrupts.h>
#include <mem/heap.h>
#include <mem/paging.h>
#include <modules/mpx_supt.h>

#include "term/commhand.c"
#include "term/dispatch/context.h"
#include "term/pcb/pcb.h"
#include "term/dnt/dnt.h"
#include "term/memory_management/mm.h"

void kmain(void)
{
   extern uint32_t magic;
   // Uncomment if you want to access the multiboot header
   // extern void *mbd;
   // char *boot_loader_name = (char*)((long*)mbd)[16];

   // ASCII art because we are trying to build a brand
   //mama();
  
   // 0) Initialize Serial I/O 
   // functions to initialize serial I/O can be found in serial.c
   // there are 3 functions to call

   init_serial(COM1);
   set_serial_in(COM1);
   set_serial_out(COM1);

   // Memory managers have been written and enabled
   mpx_init(MEM_MODULE);

   // Initialize dynamic memory
   initHeap(50000);
   sys_set_malloc(&allocateMemory);
   sys_set_free(&freeMemory);

 
   klogv("Starting MPX boot sequence...");
   klogv("Initialized serial I/O on COM1 device...");

   // 1) Initialize the support software by identifying the current
   //     MPX Module.  This will change with each module.
   // you will need to call mpx_init from the mpx_supt.c
 	
   // 2) Check that the boot was successful and correct when using grub
   // Comment this when booting the kernel directly using QEMU, etc.
   if ( magic != 0x2BADB002 ){
     //kpanic("Boot was not error free. Halting.");
   }
   
   // 3) Descriptor Tables -- tables.c
   //  you will need to initialize the global
   // this keeps track of allocated segments and pages
   klogv("Initializing descriptor tables...");

   init_gdt();
   init_idt();

   // No place for the programmable interrupt controller so I'm putting it here
   // with the 32 IRQ
   init_pic();
   init_irq();
   sti();

   // Initialize PCB queues
   initPCB();

    // 4)  Interrupt vector table --  tables.c
    // this creates and initializes a default interrupt vector table
    // this function is in tables.c
    
    klogv("Interrupt vector table initialized!");
    
   // 5) Virtual Memory -- paging.c  -- init_paging
   //  this function creates the kernel's heap
   //  from which memory will be allocated when the program calls
   // sys_alloc_mem UNTIL the memory management module  is completed
   // this allocates memory using discrete "pages" of physical memory
   // NOTE:  You will only have about 70000 bytes of dynamic memory
   //
   klogv("Initializing virtual memory...");

   init_paging();

   // 6) Call YOUR command handler -  interface method
   klogv("Transferring control to commhand...");
   // commhand(); // !!! ENABLE/RE-ENABLE FOR R4 !!!

   // Commhand PCB
   pcb_t * commhandPCB = dispatcher("commhand",&commhand);
   commhandPCB->pcb_priority = 9;
   commhandPCB->pcb_process_class = 0;
   commhandPCB->pcb_process_state = READY;
   insertPCB(commhandPCB);

   // Alarm PCB
   pcb_t * alarmPCB = dispatcher("alarms", &dispatchAlarm);
   alarmPCB->pcb_priority = 4;
   alarmPCB->pcb_process_class = 0;
   alarmPCB->pcb_process_state = READY;
   insertPCB(alarmPCB);
   
   // Idle PCB
   pcb_t * idlePCB = dispatcher("idle",&idle);
   idlePCB->pcb_priority = 1;
   idlePCB->pcb_process_class = 0;
   idlePCB->pcb_process_state = READY;
   idlePCB->pcb_protection_mode = DELETABLE_WHEN_SUSPENDED;
   insertPCB(idlePCB);
  
   // yield
   yield();

   // 7) System Shutdown on return from your command handler
   klogv("Starting system shutdown procedure...");

   /* Shutdown Procedure */
   klogv("Shutdown complete. You may now turn off the machine. (QEMU: C-a x)");
   hlt();
}
