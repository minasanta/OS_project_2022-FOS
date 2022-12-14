/*
 * commands.c
 *
 *  Created on: Oct 14, 2022
 *      Author: HP
 *      Contains the commands array & the command functions
 */

#include "commands.h"

#include <kern/trap/trap.h>
#include <kern/trap/fault_handler.h>
#include <kern/proc/user_environment.h>
#include <kern/proc/priority_manager.h>
#include "../cpu/sched.h"
#include "../disk/pagefile_manager.h"
#include "../mem/kheap.h"
#include "../mem/memory_manager.h"
#include "../tests/test_dynamic_allocator.h"
#include "../tests/test_kheap.h"
#include "../tests/test_priority.h"
#include "../tests/test_commands.h"
#include "../tests/utilities.h"


//Array of commands. (initialized)
struct Command commands[] =
{
		{ "help", "Display this list of commands", command_help },
		{ "kernel_info", "Display information about the kernel", command_kernel_info },

		//TODO: LAB2 Hands-on: add the commands here


		//LAB3: Examples
		{ "ikb", "Lab3.Example: shows mapping info of KERNEL_BASE" ,command_kernel_base_info},
		{ "dkb", "Lab3.Example: delete the mapping of KERNEL_BASE" ,command_del_kernel_base},
		{ "shr", "Lab3.Example: share one page on another" ,command_share_page},

		//LAB4: Hands-on
		{ "sm", "Lab4.HandsOn: display the mapping info for the given virtual address", command_show_mapping},
		{ "sp", "Lab4.HandsOn: set the desired permission to a given virtual address page", command_set_permission},
		{ "sr", "Lab4.HandsOn: shares the physical frames of the first virtual range with the 2nd virtual range", command_share_range},

		//LAB5: Examples
		{ "nr", "Lab5.Example: show the number of references of the physical frame" ,command_nr},
		{ "ap", "Lab5.Example: allocate one page [if not exists] in the user space at the given virtual address", command_ap},
		{ "fp", "Lab5.Example: free one page in the user space at the given virtual address", command_fp},

		//LAB5: Hands-on
		{ "asp", "Lab5.HandsOn: allocate 2 shared pages with the given virtual addresses" ,command_asp},
		{ "cfp", "Lab5.HandsOn: count the number of free pages in the given range", command_cfp},

		//LAB6: Examples
		{ "ft", "Lab6.Example: Free table", command_ft},
		{ "run", "runs a single user program", command_run_program },
		{ "kill", "kill the given environment (by its ID) from the system", command_kill_program},

		{ "wum", "writes one byte to specific location in given environment" ,command_writeusermem},
		{ "rum", "reads one byte from specific location in given environment" ,command_readusermem},
		{ "wm", "writes one byte to specific location" ,command_writemem_k},
		{ "rm", "reads one byte from specific location" ,command_readmem_k},
		{ "rub", "reads block of bytes from specific location in given environment" ,command_readuserblock},
		{ "rut", "", command_remove_table},
		{ "aup", "", command_allocuserpage},
		{ "meminfo", "", command_meminfo},

		{ "schedMLFQ", "switch the scheduler to MLFQ with given # queues & quantums", command_sch_MLFQ},
		{ "schedRR", "switch the scheduler to RR with given quantum", command_sch_RR},
		{"sched?", "print current scheduler algorithm", command_print_sch_method},
		{"schedTest", "Used for turning on/off the scheduler test", command_sch_test},

		{"load", "load a single user program to mem with status = NEW", commnad_load_env},
		{"runall", "run all loaded programs", command_run_all},
		{"printall", "print all loaded programs", command_print_all},
		{"killall", "kill all environments in the system", command_kill_all},
		{"lru", "set replacement algorithm to LRU", command_set_page_rep_LRU},
		{"fifo", "set replacement algorithm to FIFO", command_set_page_rep_FIFO},
		{"clock", "set replacement algorithm to CLOCK", command_set_page_rep_CLOCK},
		{"modifiedclock", "set replacement algorithm to modified CLOCK", command_set_page_rep_ModifiedCLOCK},
		{"nclock", "set replacement algorithm to Nth chance CLOCK", command_set_page_rep_nthCLOCK},
		{"rep?", "print current replacement algorithm", command_print_page_rep},

		{"uhfirstfit", "set USER heap placement strategy to FIRST FIT", command_set_uheap_plac_FIRSTFIT},
		{"uhbestfit", "set USER heap placement strategy to BEST FIT", command_set_uheap_plac_BESTFIT},
		{"uhnextfit", "set USER heap placement strategy to NEXT FIT", command_set_uheap_plac_NEXTFIT},
		{"uhworstfit", "set USER heap placement strategy to WORST FIT", command_set_uheap_plac_WORSTFIT},
		{"uheap?", "print current USER heap placement strategy", command_print_uheap_plac},

		{"khcontalloc", "set KERNEL heap placement strategy to CONTINUOUS ALLOCATION", command_set_kheap_plac_CONTALLOC},
		{"khfirstfit", "set KERNEL heap placement strategy to FIRST FIT", command_set_kheap_plac_FIRSTFIT},
		{"khbestfit", "set KERNEL heap placement strategy to BEST FIT", command_set_kheap_plac_BESTFIT},
		{"khnextfit", "set KERNEL heap placement strategy to NEXT FIT", command_set_kheap_plac_NEXTFIT},
		{"khworstfit", "set KERNEL heap placement strategy to WORST FIT", command_set_kheap_plac_WORSTFIT},
		{"kheap?", "print current KERNEL heap placement strategy", command_print_kheap_plac},

		//2016
		{"nobuff", "", command_disable_buffering},
		{"buff", "", command_enable_buffering},

		{"nomodbuff", "", command_disable_modified_buffer},
		{"modbuff", "", command_enable_modified_buffer},

		{"modbufflength?", "", command_get_modified_buffer_length},
		{"modbufflength", "", command_set_modified_buffer_length},

		{"tstkmalloc", "Kernel Heap: test kmalloc (return address, size, mem access...etc)", command_test_kmalloc},
		{"tstkfree", "Kernel Heap: test kfree (freed frames, mem access...etc)", command_test_kfree},
		{"tstkphysaddr", "Kernel Heap: test kheap_phys_addr", command_test_kheap_phys_addr},
		{"tstkvirtaddr", "Kernel Heap: test kheap_virt_addr", command_test_kheap_virt_addr},
		{"tst3functions", "Env Load: test the creation of new dir, tables and pages WS", command_test_three_creation_functions},
		{"tstkfreeall", "Kernel Heap: test kfreeall (freed frames, mem access...etc)", command_test_kfreeall},
		{"tstkexpand", "Kernel Heap: test expanding last allocated var", command_test_kexpand},
		{"tstkshrink", "Kernel Heap: test srinking last allocated var", command_test_kshrink},
		{"tstkfreelast", "Kernel Heap: test freeing last allocated var", command_test_kfreelast},
		{"tstkrealloc","Kernel realloc: test realloc (virtual address = 0)",command_test_krealloc },
		{"tstpriority1", "Tests the priority of the program (Normal and Higher)", command_test_priority1},
		{"tstpriority2", "Tests the priority of the program (Normal and Lower)", command_test_priority2},
		{"tstsc4","Scenario#4: MLFQ",command_test_sc_MLFQ },
		//2022
		{"autocomplete", "Test commands autcomplte", command_test_autocomplete},
		{"tstlsts","Test lists manipulation for dynamic blocks management", command_test_lists },
		{"tstpg", "Test paging manipulation for a specific page", command_test_paging_manipulation},
		{"tstchunks","Test chunk manipulations", command_test_chunks },
		{"tstkheap", "Test KHEAP functions", command_test_kheap}
};

//Number of commands = size of the array / size of command structure
uint32 NUM_OF_COMMANDS  = (sizeof(commands)/sizeof(struct Command));


/***** Implementations of basic kernel command prompt commands *****/

//print name and description of each command
int command_help(int number_of_arguments, char **arguments)
{
	int i;
	for (i = 0; i < NUM_OF_COMMANDS; i++)
		cprintf("%s - %s\n", commands[i].name, commands[i].description);

	cprintf("-------------------\n");

	for (i = 0; i < NUM_USER_PROGS; i++)
		cprintf("run %s - %s [User Program]\n", ptr_UserPrograms[i].name, ptr_UserPrograms[i].desc);
	return 0;
}

//print information about kernel addresses and kernel size
int command_kernel_info(int number_of_arguments, char **arguments )
{
	extern char start_of_kernel[], end_of_kernel_code_section[], start_of_uninitialized_data_section[], end_of_kernel[];

	cprintf("Special kernel symbols:\n");
	cprintf("  Start Address of the kernel 			%08x (virt)  %08x (phys)\n", start_of_kernel, start_of_kernel - KERNEL_BASE);
	cprintf("  End address of kernel code  			%08x (virt)  %08x (phys)\n", end_of_kernel_code_section, end_of_kernel_code_section - KERNEL_BASE);
	cprintf("  Start addr. of uninitialized data section 	%08x (virt)  %08x (phys)\n", start_of_uninitialized_data_section, start_of_uninitialized_data_section - KERNEL_BASE);
	cprintf("  End address of the kernel   			%08x (virt)  %08x (phys)\n", end_of_kernel, end_of_kernel - KERNEL_BASE);
	cprintf("Kernel executable memory footprint: %d KB\n",
			(end_of_kernel-start_of_kernel+1023)/1024);
	return 0;
}

//===========================================================================
//Lab2.Hands.On
//=============
//TODO: LAB2 Hands-on: write the command function here


//===========================================================================
//Lab3.Examples
//=============
int command_kernel_base_info(int number_of_arguments, char **arguments)
{
	//TODO: LAB3 Example: fill this function. corresponding command name is "ikb"
	//Comment the following line
	panic("Function is not implemented yet!");

	return 0;
}


int command_del_kernel_base(int number_of_arguments, char **arguments)
{
	//TODO: LAB3 Example: fill this function. corresponding command name is "dkb"
	//Comment the following line
	panic("Function is not implemented yet!");

	return 0;
}

int command_share_page(int number_of_arguments, char **arguments)
{
	//TODO: LAB3 Example: fill this function. corresponding command name is "shr"
	//Comment the following line
	panic("Function is not implemented yet!");

	return 0;
}

//===========================================================================
//Lab4.Hands.On
//=============
int command_show_mapping(int number_of_arguments, char **arguments)
{
	//TODO: LAB4 Hands-on: fill this function. corresponding command name is "sm"
	//Comment the following line
	panic("Function is not implemented yet!");

	return 0 ;
}

int command_set_permission(int number_of_arguments, char **arguments)
{
	//TODO: LAB4 Hands-on: fill this function. corresponding command name is "sp"
	//Comment the following line
	panic("Function is not implemented yet!");

	return 0 ;
}

int command_share_range(int number_of_arguments, char **arguments)
{
	//TODO: LAB4 Hands-on: fill this function. corresponding command name is "sr"
	//Comment the following line
	panic("Function is not implemented yet!");

	return 0;
}

//===========================================================================
//Lab5.Examples
//==============
//[1] Number of references on the given physical address
int command_nr(int number_of_arguments, char **arguments)
{
	//TODO: LAB5 Example: fill this function. corresponding command name is "nr"
	//Comment the following line
	panic("Function is not implemented yet!");

	return 0;
}

//[2] Allocate Page: If the given user virtual address is mapped, do nothing. Else, allocate a single frame and map it to a given virtual address in the user space
int command_ap(int number_of_arguments, char **arguments)
{
	//TODO: LAB5 Example: fill this function. corresponding command name is "ap"
	//Comment the following line
	//panic("Function is not implemented yet!");

	uint32 va = strtol(arguments[1], NULL, 16);
	struct FrameInfo* ptr_frame_info;
	int ret = allocate_frame(&ptr_frame_info) ;
	map_frame(ptr_page_directory, ptr_frame_info, va, PERM_USER | PERM_WRITEABLE);

	return 0 ;
}

//[3] Free Page: Un-map a single page at the given virtual address in the user space
int command_fp(int number_of_arguments, char **arguments)
{
	//TODO: LAB5 Example: fill this function. corresponding command name is "fp"
	//Comment the following line
	//panic("Function is not implemented yet!");

	uint32 va = strtol(arguments[1], NULL, 16);
	// Un-map the page at this address
	unmap_frame(ptr_page_directory, va);

	return 0;
}

//===========================================================================
//Lab5.Hands-on
//==============
//[1] Allocate Shared Pages
int command_asp(int number_of_arguments, char **arguments)
{
	//TODO: LAB5 Hands-on: fill this function. corresponding command name is "asp"
	//Comment the following line
	panic("Function is not implemented yet!");

	return 0;
}


//[2] Count Free Pages in Range
int command_cfp(int number_of_arguments, char **arguments)
{
	//TODO: LAB5 Hands-on: fill this function. corresponding command name is "cfp"
	//Comment the following line
	panic("Function is not implemented yet!");

	return 0;
}

//===========================================================================
//Lab6.Examples
//=============
int command_ft(int number_of_arguments, char **arguments)
{
	//TODO: LAB6 Example: fill this function. corresponding command name is "ft"
	//Comment the following line

	return 0;
}

//*****************************************************************************************//
//***************************** PROJECT HELPERS COMMAND ***********************************//
//*****************************************************************************************//
int command_writeusermem(int number_of_arguments, char **arguments)
{
	//deal with the kernel page directory
	if (number_of_arguments == 3)
	{
		unsigned int address = strtol(arguments[1], NULL, 16);
		unsigned char *ptr = (unsigned char *)(address) ;

		*ptr = arguments[2][0];
	}
	//deal with a page directory of specific environment
	else if (number_of_arguments == 4)
	{
		int32 envId = strtol(arguments[1],NULL, 10);
		struct Env* env = NULL;
		envid2env(envId, &env, 0 );

		int address = strtol(arguments[2], NULL, 16);

		if(env == NULL) return 0;

		uint32 oldDir = rcr3();
		//lcr3((uint32) K_PHYSICAL_ADDRESS( env->env_pgdir));
		lcr3((uint32) (env->env_cr3));

		unsigned char *ptr = (unsigned char *)(address) ;

		//Write the given Character
		*ptr = arguments[3][0];
		lcr3(oldDir);
	}
	else
	{
		cprintf("wum command: invalid number of arguments\n") ;
	}
	return 0;
}

int command_writemem_k(int number_of_arguments, char **arguments)
{
	unsigned char* address = (unsigned char*)strtol(arguments[1], NULL, 16)+KERNEL_BASE;
	int size = strtol(arguments[2], NULL, 10);
	int c, i=0;
	int stringLen = strlen(arguments[3]);

	for(c=0; c<size; c++)
	{
		for(i=0;i < stringLen; i++)
		{
			*address = arguments[3][i];
			address++;
		}
	}
	return 0;


}

int command_readusermem(int number_of_arguments, char **arguments)
{
	//deal with the kernel page directory
	if (number_of_arguments == 2)
	{
		unsigned int address = strtol(arguments[1], NULL, 16);
		unsigned char *ptr = (unsigned char *)(address ) ;

		cprintf("value at address %x = %c\n", ptr, *ptr);
	}
	//deal with a page directory of specific environment
	else if (number_of_arguments == 3)
	{
		int32 envId = strtol(arguments[1],NULL, 10);
		struct Env* env = NULL;
		envid2env(envId, &env, 0 );

		int address = strtol(arguments[2], NULL, 16);

		if(env == NULL) return 0;

		uint32 oldDir = rcr3();
		//lcr3((uint32) K_PHYSICAL_ADDRESS( env->env_pgdir));
		lcr3((uint32)( env->env_cr3));

		unsigned char *ptr = (unsigned char *)(address) ;

		//Write the given Character
		cprintf("value at address %x = %c\n", address, *ptr);

		lcr3(oldDir);
	}
	else
	{
		cprintf("rum command: invalid number of arguments\n") ;
	}
	return 0;

}

int command_readmem_k(int number_of_arguments, char **arguments)
{
	unsigned char* address = (unsigned char*)strtol(arguments[1], NULL, 16)+KERNEL_BASE;
	int size = strtol(arguments[2], NULL, 10);
	int i=0;
	for(;i < size; i++)
	{
		cprintf("%c",*address++);
	}
	cprintf("\n");
	return 0;
}


int command_readuserblock(int number_of_arguments, char **arguments)
{
	int32 envId = strtol(arguments[1],NULL, 10);
	struct Env* env = NULL;
	envid2env(envId, &env, 0 );

	int address = strtol(arguments[2], NULL, 16);
	int nBytes = strtol(arguments[3], NULL, 10);

	unsigned char *ptr = (unsigned char *)(address) ;
	//Write the given Character

	if(env == NULL) return 0;

	uint32 oldDir = rcr3();
	//lcr3((uint32) K_PHYSICAL_ADDRESS( env->env_pgdir));
	lcr3((uint32)( env->env_cr3));

	int i;
	for(i = 0;i<nBytes; i++)
	{
		cprintf("%08x : %02x  %c\n", ptr, *ptr, *ptr);
		ptr++;
	}
	lcr3(oldDir);

	return 0;
}

int command_remove_table(int number_of_arguments, char **arguments)
{
	int32 envId = strtol(arguments[1],NULL, 10);
	struct Env* env = NULL;
	envid2env(envId, &env, 0 );
	if(env == 0) return 0;

	uint32 address = strtol(arguments[2], NULL, 16);
	unsigned char *va = (unsigned char *)(address) ;
	uint32 table_pa = env->env_page_directory[PDX(address)] & 0xFFFFF000;

	//remove the table
	if(USE_KHEAP && !CHECK_IF_KERNEL_ADDRESS(va))
	{
		kfree((void*)kheap_virtual_address(table_pa));
	}
	else
	{
		// get the physical address and FrameInfo of the page table
		struct FrameInfo *table_FrameInfo = to_frame_info(table_pa);
		// set references of the table frame to 0 then free it by adding
		// to the free frame list
		table_FrameInfo->references = 0;
		free_frame(table_FrameInfo);
	}

	// set the corresponding entry in the directory to 0
	uint32 dir_index = PDX(va);
	env->env_page_directory[dir_index] &= (~PERM_PRESENT);
	tlbflush();
	return 0;
}

int command_allocuserpage(int number_of_arguments, char **arguments)
{
	if (number_of_arguments < 3 || number_of_arguments > 4)
	{
		cprintf("aup command: invalid number of arguments\n") ;
		return 0;
	}
	int32 envId = strtol(arguments[1],NULL, 10);
	struct Env* env = NULL;

	envid2env(envId, &env, 0 );
	if(env == 0) return 0;

	uint32 va = strtol(arguments[2], NULL, 16);

	// Allocate a single frame from the free frame list
	struct FrameInfo * ptr_FrameInfo ;
	int ret = allocate_frame(&ptr_FrameInfo);
	if (ret == E_NO_MEM)
	{
		cprintf("ERROR: no enough memory\n");
		return 0;
	}

	if (number_of_arguments == 3)
	{
		// Map this frame to the given user virtual address with PERM_WRITEABLE
		map_frame(env->env_page_directory, ptr_FrameInfo, va, PERM_WRITEABLE | PERM_USER);
	}
	else if (number_of_arguments == 4)
	{
		// Map this frame to the given user virtual address with the given permission
		uint32 rw ;
		if (arguments[3][0] == 'r' || arguments[3][0] == 'R')
			rw = 0 ;
		else if (arguments[3][0] == 'w' || arguments[3][0] == 'W')
			rw = PERM_WRITEABLE ;
		else
		{
			cprintf("aup command: wrong permission (r/w)... will continue as writable\n") ;
			rw = PERM_WRITEABLE ;
		}

		map_frame(env->env_page_directory, ptr_FrameInfo, va, rw | PERM_USER);
	}
	return 0;
}

int command_meminfo(int number_of_arguments, char **arguments)
{
	struct freeFramesCounters counters =calculate_available_frames();
	cprintf("Total available frames = %d\nFree Buffered = %d\nFree Not Buffered = %d\nModified = %d\n",
			counters.freeBuffered+ counters.freeNotBuffered+ counters.modified, counters.freeBuffered, counters.freeNotBuffered, counters.modified);

	cprintf("Num of calls for kheap_virtual_address [in last run] = %d\n", numOfKheapVACalls);

	return 0;
}

//2020
struct Env * CreateEnv(int number_of_arguments, char **arguments)
{
	struct Env* env;
	uint32 pageWSSize = __PWS_MAX_SIZE;		//arg#3 default
	uint32 LRUSecondListSize = 0;			//arg#4 default
	uint32 percent_WS_pages_to_remove = 0;	//arg#5 default
#if USE_KHEAP
	{
		switch (number_of_arguments)
		{
		case 5:
			if(!isPageReplacmentAlgorithmLRU(PG_REP_LRU_LISTS_APPROX))
			{
				cprintf("ERROR: Current Replacement is NOT LRU LISTS, invalid number of args\nUsage: <command> <prog_name> <page_WS_size> [<LRU_second_list_size>] [<DYN_LOC_SCOPE_percent_WS_to_remove>]\naborting...\n");
				return NULL;
			}
			percent_WS_pages_to_remove = strtol(arguments[4], NULL, 10);
			LRUSecondListSize = strtol(arguments[3], NULL, 10);
			pageWSSize = strtol(arguments[2], NULL, 10);
			break;
		case 4:
			if(!isPageReplacmentAlgorithmLRU(PG_REP_LRU_LISTS_APPROX))
			{
				percent_WS_pages_to_remove = strtol(arguments[3], NULL, 10);
			}
			else
			{
				LRUSecondListSize = strtol(arguments[3], NULL, 10);
			}
			pageWSSize = strtol(arguments[2], NULL, 10);
			break;
		case 3:
			if(isPageReplacmentAlgorithmLRU(PG_REP_LRU_LISTS_APPROX))
			{
				cprintf("ERROR: Current Replacement is LRU LISTS, Please specify a working set size in the 3rd arg and LRU second list size in the 4th arg, aborting.\n");
				return NULL;
			}
			pageWSSize = strtol(arguments[2], NULL, 10);
			break;
		default:
			cprintf("ERROR: invalid number of args\nUsage: <command> <prog_name> <page_WS_size> [<LRU_second_list_size>] [<DYN_LOC_SCOPE_percent_WS_to_remove>]\naborting...\n");
			return NULL;

			break;
		}
		if(pageWSSize > __PWS_MAX_SIZE)
		{
			cprintf("ERROR: size of WS must be less than or equal to %d... aborting", __PWS_MAX_SIZE);
			return NULL;
		}
		if(isPageReplacmentAlgorithmLRU(PG_REP_LRU_LISTS_APPROX))
		{
			if (LRUSecondListSize > pageWSSize - 1)
			{
				cprintf("ERROR: size of LRU second list can't equal/exceed the size of the page WS... aborting\n");
				return NULL;
			}
		}
		assert(percent_WS_pages_to_remove >= 0 && percent_WS_pages_to_remove <= 100);

	}
#else
	{
		switch (number_of_arguments)
		{
		case 3:
			percent_WS_pages_to_remove = strtol(arguments[2], NULL, 10);
		case 2:
			break;
		default:
			cprintf("ERROR: invalid number of args\nUsage: <command> <prog_name> [<DYN_LOC_SCOPE_percent_WS_to_remove>]\naborting...\n");
			return NULL;

			break;
		}
		if(isPageReplacmentAlgorithmLRU(PG_REP_LRU_LISTS_APPROX))
		{
			LRUSecondListSize = __LRU_SNDLST_SIZE;
		}
	}
#endif
	assert(percent_WS_pages_to_remove >= 0 && percent_WS_pages_to_remove <= 100);
	env = env_create(arguments[1], pageWSSize, LRUSecondListSize, percent_WS_pages_to_remove);

	return env;
}

int command_run_program(int number_of_arguments, char **arguments)
{
	//[1] Create and initialize a new environment for the program to be run
	struct Env *env = CreateEnv(number_of_arguments, arguments);

	if(env == NULL) return 0;
	cprintf("\nEnvironment Id= %d\n",env->env_id);

	numOfKheapVACalls = 0;

	//[2] Run the created environment by adding it to the "ready" queue then invoke the scheduler to execute it
	sched_new_env(env);
	sched_run_env(env->env_id);

	return 0;
}

int command_kill_program(int number_of_arguments, char **arguments)
{
	int32 envId = strtol(arguments[1],NULL, 10);

	sched_kill_env(envId);

	return 0;
}

int commnad_load_env(int number_of_arguments, char **arguments)
{
	struct Env *env = CreateEnv(number_of_arguments, arguments);
	if (env == NULL)
		return 0 ;

	sched_new_env(env) ;

	cprintf("\nEnvironment Id= %d\n",env->env_id);
	return 0;
}

int command_run_all(int number_of_arguments, char **arguments)
{
	numOfKheapVACalls = 0;
	sched_run_all();

	return 0 ;
}

int command_print_all(int number_of_arguments, char **arguments)
{
	sched_print_all();

	return 0 ;
}

int command_kill_all(int number_of_arguments, char **arguments)
{
	sched_kill_all();

	return 0 ;
}

int command_set_page_rep_LRU(int number_of_arguments, char **arguments)
{
	if (number_of_arguments < 2)
	{
		cprintf("ERROR: please specify the LRU Approx Type (1: TimeStamp Approx, 2: Lists Approx), aborting...\n");
		return 0;
	}
	int LRU_TYPE = strtol(arguments[1], NULL, 10) ;
	if (LRU_TYPE == PG_REP_LRU_TIME_APPROX)
	{
		setPageReplacmentAlgorithmLRU(LRU_TYPE);
		cprintf("Page replacement algorithm is now LRU with TimeStamp approximation\n");
	}
	else if (LRU_TYPE == PG_REP_LRU_LISTS_APPROX)
	{
		setPageReplacmentAlgorithmLRU(LRU_TYPE);
		cprintf("Page replacement algorithm is now LRU with LISTS approximation\n");
	}
	else
	{
		cprintf("ERROR: Invalid LRU Approx Type (1: TimeStamp Approx, 2: Lists Approx), aborting...\n");
		return 0;
	}
	return 0;
}
//2021
int command_set_page_rep_nthCLOCK(int number_of_arguments, char **arguments)
{
	uint32 PageWSMaxSweeps = strtol(arguments[1], NULL, 10);
	setPageReplacmentAlgorithmNchanceCLOCK(PageWSMaxSweeps);
	cprintf("Page replacement algorithm is now N chance CLOCK\n");
	return 0;
}
int command_set_page_rep_CLOCK(int number_of_arguments, char **arguments)
{
	setPageReplacmentAlgorithmCLOCK();
	cprintf("Page replacement algorithm is now CLOCK\n");
	return 0;
}

int command_set_page_rep_FIFO(int number_of_arguments, char **arguments)
{
	setPageReplacmentAlgorithmFIFO();
	cprintf("Page replacement algorithm is now FIFO\n");
	return 0;
}

int command_set_page_rep_ModifiedCLOCK(int number_of_arguments, char **arguments)
{
	setPageReplacmentAlgorithmModifiedCLOCK();
	cprintf("Page replacement algorithm is now Modified CLOCK\n");
	return 0;
}

/*2018*///BEGIN======================================================
int command_sch_RR(int number_of_arguments, char **arguments)
{
	uint8 quantum = strtol(arguments[1], NULL, 10);

	sched_init_RR(quantum);
	cprintf("Scheduler is now set to Round Robin with quantum %d ms\n", quantums[0]);
	return 0;
}
int command_sch_MLFQ(int number_of_arguments, char **arguments)
{
	uint8 numOfLevels = strtol(arguments[1], NULL, 10);
	uint8 quantumOfEachLevel[MAX_ARGUMENTS - 2] ;
	for (int i = 2 ; i < number_of_arguments ; i++)
	{
		quantumOfEachLevel[i-2] = strtol(arguments[i], NULL, 10);
	}

	sched_init_MLFQ(numOfLevels, quantumOfEachLevel);

	cprintf("Scheduler is now set to MLFQ with quantums: ");
	for (int i = 0 ; i < num_of_ready_queues; i++)
	{
		cprintf("%d   ", quantums[i]) ;
	}
	cprintf("\n");
	return 0;
}
int command_print_sch_method(int number_of_arguments, char **arguments)
{
	if (isSchedMethodMLFQ())
	{
		cprintf("Current scheduler method is MLFQ with quantums: ");
		for (int i = 0 ; i < num_of_ready_queues; i++)
		{
			cprintf("%d   ", quantums[i]) ;
		}
		cprintf("\n");
	}
	else if (isSchedMethodRR())
	{
		cprintf("Current scheduler method is Round Robin with quantum %d ms\n", quantums[0]);
	}

	else
		cprintf("Current scheduler method is UNDEFINED\n");

	return 0;
}
int command_sch_test(int number_of_arguments, char **arguments)
{
	int status  = strtol(arguments[1], NULL, 10);
	chksch(status);
	if (status == 0)
		cprintf("Testing the scheduler is TURNED OFF\n");
	else if (status == 1)
		cprintf("Testing the scheduler is TURNED ON\n");
	return 0;
}

/*2018*///END======================================================


/*2015*///BEGIN======================================================
int command_print_page_rep(int number_of_arguments, char **arguments)
{
	if (isPageReplacmentAlgorithmCLOCK())
		cprintf("Page replacement algorithm is CLOCK\n");
	else if (isPageReplacmentAlgorithmLRU(PG_REP_LRU_TIME_APPROX))
		cprintf("Page replacement algorithm is LRU with TimeStamp approximation\n");
	else if (isPageReplacmentAlgorithmLRU(PG_REP_LRU_LISTS_APPROX))
		cprintf("Page replacement algorithm is LRU with LISTS approximation\n");
	else if (isPageReplacmentAlgorithmFIFO())
		cprintf("Page replacement algorithm is FIFO\n");
	else if (isPageReplacmentAlgorithmModifiedCLOCK())
		cprintf("Page replacement algorithm is Modified CLOCK\n");
	else
		cprintf("Page replacement algorithm is UNDEFINED\n");

	return 0;
}


int command_set_uheap_plac_FIRSTFIT(int number_of_arguments, char **arguments)
{
	setUHeapPlacementStrategyFIRSTFIT();
	cprintf("User Heap placement strategy is now FIRST FIT\n");
	return 0;
}

int command_set_uheap_plac_BESTFIT(int number_of_arguments, char **arguments)
{
	setUHeapPlacementStrategyBESTFIT();
	cprintf("User Heap placement strategy is now BEST FIT\n");
	return 0;
}

int command_set_uheap_plac_NEXTFIT(int number_of_arguments, char **arguments)
{
	setUHeapPlacementStrategyNEXTFIT();
	cprintf("User Heap placement strategy is now NEXT FIT\n");
	return 0;
}
int command_set_uheap_plac_WORSTFIT(int number_of_arguments, char **arguments)
{
	setUHeapPlacementStrategyWORSTFIT();
	cprintf("User Heap placement strategy is now WORST FIT\n");
	return 0;
}

int command_print_uheap_plac(int number_of_arguments, char **arguments)
{
	if (isUHeapPlacementStrategyFIRSTFIT())
		cprintf("User Heap placement strategy is FIRST FIT\n");
	else if (isUHeapPlacementStrategyBESTFIT())
		cprintf("User Heap placement strategy is BEST FIT\n");
	else if (isUHeapPlacementStrategyNEXTFIT())
		cprintf("User Heap placement strategy is NEXT FIT\n");
	else if (isUHeapPlacementStrategyWORSTFIT())
		cprintf("User Heap placement strategy is WORST FIT\n");
	else
		cprintf("User Heap placement strategy is UNDEFINED\n");

	return 0;
}

/*2015*///END======================================================

/*2017*///BEGIN======================================================

int command_set_kheap_plac_CONTALLOC(int number_of_arguments, char **arguments)
{
	setKHeapPlacementStrategyCONTALLOC();
	cprintf("Kernel Heap placement strategy is now FIRST FIT\n");
	return 0;
}

int command_set_kheap_plac_FIRSTFIT(int number_of_arguments, char **arguments)
{
	setKHeapPlacementStrategyFIRSTFIT();
	cprintf("Kernel Heap placement strategy is now FIRST FIT\n");
	return 0;
}

int command_set_kheap_plac_BESTFIT(int number_of_arguments, char **arguments)
{
	setKHeapPlacementStrategyBESTFIT();
	cprintf("Kernel Heap placement strategy is now BEST FIT\n");
	return 0;
}

int command_set_kheap_plac_NEXTFIT(int number_of_arguments, char **arguments)
{
	setKHeapPlacementStrategyNEXTFIT();
	cprintf("Kernel Heap placement strategy is now NEXT FIT\n");
	return 0;
}
int command_set_kheap_plac_WORSTFIT(int number_of_arguments, char **arguments)
{
	setKHeapPlacementStrategyWORSTFIT();
	cprintf("Kernel Heap placement strategy is now WORST FIT\n");
	return 0;
}

int command_print_kheap_plac(int number_of_arguments, char **arguments)
{
	if (isKHeapPlacementStrategyCONTALLOC())
		cprintf("Kernel Heap placement strategy is CONTINUOUS ALLOCATION\n");
	else if (isKHeapPlacementStrategyFIRSTFIT())
		cprintf("Kernel Heap placement strategy is FIRST FIT\n");
	else if (isKHeapPlacementStrategyBESTFIT())
		cprintf("Kernel Heap placement strategy is BEST FIT\n");
	else if (isKHeapPlacementStrategyNEXTFIT())
		cprintf("Kernel Heap placement strategy is NEXT FIT\n");
	else if (isKHeapPlacementStrategyWORSTFIT())
		cprintf("Kernel Heap placement strategy is WORST FIT\n");
	else
		cprintf("Kernel Heap placement strategy is UNDEFINED\n");

	return 0;
}

/*2017*///END======================================================

int command_disable_modified_buffer(int number_of_arguments, char **arguments)
{
	if(!isBufferingEnabled())
	{
		cprintf("Buffering is not enabled. Please enable buffering first.\n");
	}
	else
	{
		enableModifiedBuffer(0);
		cprintf("Modified Buffer is now DISABLED\n");
	}
	return 0;
}


int command_enable_modified_buffer(int number_of_arguments, char **arguments)
{
	if(!isBufferingEnabled())
	{
		cprintf("Buffering is not enabled. Please enable buffering first.\n");
	}
	else
	{
		enableModifiedBuffer(1);
		cprintf("Modified Buffer is now ENABLED\n");
	}
	return 0;
}

/*2016 ============================================================================*/

int command_disable_buffering(int number_of_arguments, char **arguments)
{
	enableBuffering(0);
	enableModifiedBuffer(0);
	cprintf("Buffering is now DISABLED\n");
	return 0;
}


int command_enable_buffering(int number_of_arguments, char **arguments)
{
	enableBuffering(1);
	enableModifiedBuffer(1);
	if(getModifiedBufferLength() == 0)
	{
		cprintf("Modified buffer enabled but with length = 0\n");
		char str[100];
		readline("Please enter the modified buff length = ", str);
		setModifiedBufferLength(strtol(str, NULL, 10));
		cprintf("Modified buffer length updated = %d\n", getModifiedBufferLength());
	}
	cprintf("Buffering is now ENABLED\n");
	return 0;
}

int command_set_modified_buffer_length(int number_of_arguments, char **arguments)
{
	if(!isBufferingEnabled())
	{
		cprintf("Buffering is not enabled. Please enable buffering to use the modified buffer.\n");
	}
	else if (!isModifiedBufferEnabled())
	{
		cprintf("Modified Buffering is not enabled. Please enable modified buffering.\n");
	}
	setModifiedBufferLength(strtol(arguments[1], NULL, 10));
	cprintf("Modified buffer length updated = %d\n", getModifiedBufferLength());
	return 0;
}

int command_get_modified_buffer_length(int number_of_arguments, char **arguments)
{
	if(!isBufferingEnabled())
	{
		cprintf("Buffering is not enabled. Please enable buffering to use the modified buffer.\n");
	}
	else if (!isModifiedBufferEnabled())
	{
		cprintf("Modified Buffering is not enabled. Please enable modified buffering.\n");
	}
	cprintf("Modified buffer length = %d\n", getModifiedBufferLength());
	return 0;
}

/*TESTING Commands*/
int command_test_kmalloc(int number_of_arguments, char **arguments)
{
	int testNum = 0 ;
	if (number_of_arguments==2)
		testNum = strtol(arguments[1], NULL, 10);
	if (isKHeapPlacementStrategyCONTALLOC())
		test_kmalloc();
	else if (isKHeapPlacementStrategyFIRSTFIT())
	{
		if (testNum == 0)
		{
			cprintf("Error: [Kernel.FirstFit] must specify the test number (1 or 2) as an argument\n");
			return 0;
		}
		//Test FIRST FIT allocation
		if (testNum == 1)
			test_kmalloc_firstfit1();
		else if (testNum == 2)
			test_kmalloc_firstfit2();
	}
	else if (isKHeapPlacementStrategyBESTFIT())
	{
		if (testNum == 0)
		{
			cprintf("Error: [Kernel.BestFit] must specify the test number (1, 2, 3) as an argument\n");
			return 0;
		}
		if (testNum == 1)
			test_kmalloc();
		else if (testNum == 2)
			test_kmalloc_bestfit1();
		else if (testNum == 3)
			test_kmalloc_bestfit2();
	}
	else if (isKHeapPlacementStrategyNEXTFIT())
	{
		if (testNum == 0)
		{
			cprintf("Error: [Kernel.NextFit] must specify the test number (1 or 2) as an argument\n");
			return 0;
		}
		//Test cont. allocation
		if (testNum == 1)
			test_kmalloc();
		//Test nextfit strategy
		else if (testNum == 2)
			test_kmalloc_nextfit();

	}
	else if (isKHeapPlacementStrategyWORSTFIT())
		test_kmalloc_worstfit();
	return 0;
}
int command_test_kfree(int number_of_arguments, char **arguments)
{
	if (isKHeapPlacementStrategyBESTFIT() || isKHeapPlacementStrategyFIRSTFIT())
	{
		test_kfree_bestfirstfit();
	}
	else //NEXT & CONT
	{
		test_kfree();
	}
	return 0;
}
int command_test_kheap_phys_addr(int number_of_arguments, char **arguments)
{
	test_kheap_phys_addr();
	return 0;
}
int command_test_kheap_virt_addr(int number_of_arguments, char **arguments)
{
	test_kheap_virt_addr();
	return 0;
}
int command_test_three_creation_functions(int number_of_arguments, char **arguments)
{
	test_three_creation_functions();
	return 0;
}

int command_test_krealloc(int number_of_arguments, char **arguments) {
	if (isKHeapPlacementStrategyNEXTFIT())
		test_krealloc();
	else if (isKHeapPlacementStrategyBESTFIT())
		test_krealloc_BF();
	return 0;
}

int command_test_priority1(int number_of_arguments, char **arguments)
{
	test_priority_normal_and_higher();
	return 0;
}

int command_test_priority2(int number_of_arguments, char **arguments)
{
	test_priority_normal_and_lower();
	return 0;
}

int command_test_kfreeall(int number_of_arguments, char **arguments)
{
	test_kfreeall();
	return 0;
}

int command_test_kexpand(int number_of_arguments, char **arguments)
{
	test_kexpand();
	return 0;
}

int command_test_kshrink(int number_of_arguments, char **arguments)
{
	test_kshrink();
	return 0;
}

int command_test_kfreelast(int number_of_arguments, char **arguments)
{
	test_kfreelast();
	return 0;
}

int command_test_sc_MLFQ(int number_of_arguments, char **arguments)
{
	int numOfSlave2 = strtol(arguments[1], NULL, 10);
	int cnt = 0 ;
	int firstTime = 1;
	struct Env *e ;
	LIST_FOREACH(e, &env_exit_queue)
	{
		if (strcmp(e->prog_name, "tmlfq_2") == 0)
		{
			if (firstTime)
				firstTime = 0;
			cnt++ ;
		}
		else if (!firstTime)
			break;
	}
	if(cnt == numOfSlave2)
	{
		cprintf("Congratulations... MLFQScenario# completed successfully\n");
	}
	else
	{
		panic("MLFQScenario# failed\n");
	}
	return 0;
}


/*2022*/
int command_test_autocomplete(int number_of_arguments, char **arguments)
{
	int x = TestAutoCompleteCommand();
	return 0;
}
int command_test_lists(int number_of_arguments, char **arguments)
{
	if (number_of_arguments != 2)
	{
		cprintf("Invalid number of arguments! USAGE: tstlsts <testname>\n") ;
		return 0;
	}
	// Test 1 Example for initialize_MemBlocksList: tstlsts init 5
	if(strcmp(arguments[1], "init") == 0)
	{
		test_initialize_MemBlocksList();
	}
	// Test 2 Example for insert_sorted: tstlsts insrt
	else if(strcmp(arguments[1], "insrt") == 0)
	{
		test_insert_sorted_allocList();
	}
	// Test 3 Example for find_block: tstlsts find
	else if(strcmp(arguments[1], "find") == 0)
	{
		test_find_block();
	}
	// Test 4 Example for alloc_block_FF: tstlsts FF
	else if(strcmp(arguments[1], "FF") == 0 || strcmp(arguments[1], "ff") == 0)
	{
		test_alloc_block_FF();
	}
	// Test 5 Example for alloc_block_BF: tstlsts BF
	else if(strcmp(arguments[1], "BF") == 0 || strcmp(arguments[1], "bf") == 0)
	{
		test_alloc_block_BF();
	}
	// Test 6 Example for alloc_block_NF: tstlsts NF
	else if(strcmp(arguments[1], "NF") == 0 || strcmp(arguments[1], "nf") == 0)
	{
		test_alloc_block_NF();
	}
	// Test 7 Example for alloc_block_NF: tstlsts merge
	else if(strcmp(arguments[1], "MERGE") == 0 || strcmp(arguments[1], "merge") == 0)
	{
		test_insert_sorted_with_merge_freeList();
	}
	return 0;
}

int command_test_chunks(int number_of_arguments, char **arguments)
{
	if (number_of_arguments != 2)
	{
		cprintf("Invalid number of arguments! USAGE: tstchunk <testname>\n") ;
		return 0;
	}
	// CUT-PASTE Test
	if(strcmp(arguments[1], "cutpaste") == 0)
	{
		test_cut_paste_pages();
	}
	// COPY-PASTE Test
	else if(strcmp(arguments[1], "copypaste") == 0)
	{
		test_copy_paste_chunk();
	}
	// SHARE Test
	else if(strcmp(arguments[1], "share") == 0)
	{
		test_share_chunk();
	}
	// ALLOCATE Test
	else if(strcmp(arguments[1], "allocate") == 0)
	{
		test_allocate_chunk();
	}
	// REQUIRED SPACE Test
	else if(strcmp(arguments[1], "required_space") == 0)
	{
		test_calculate_required_frames();
	}
	// ALLOCATED SPACE Test
	else if(strcmp(arguments[1], "allocated_space") == 0)
	{
		test_calculate_allocated_space();
	}
	return 0;
}

int command_test_paging_manipulation(int number_of_arguments, char **arguments)
{
	if (number_of_arguments != 2)
	{
		cprintf("Invalid number of arguments! USAGE: tstpg <testname>\n") ;
		return 0;
	}
	// Test 1.1-Set/Clear permissions: tstpg scperm1
	if(strcmp(arguments[1], "scperm1") == 0)
	{
		test_pt_set_page_permissions();
	}
	// Test 1.2-Set/Clear permissions: tstpg scperm2
	else if(strcmp(arguments[1], "scperm2") == 0)
	{
		test_pt_set_page_permissions_invalid_va();
	}
	// Test 2-Get permissions: tstpg getperm
	else if(strcmp(arguments[1], "getperm") == 0)
	{
		test_pt_get_page_permissions();
	}
	// Test 3.1-Clear entry: tstpg clear1
	else if(strcmp(arguments[1], "clear1") == 0)
	{
		test_pt_clear_page_table_entry();
	}
	// Test 3.2-Clear entry: tstpg clear2
	else if(strcmp(arguments[1], "clear2") == 0)
	{
		test_pt_clear_page_table_entry_invalid_va();
	}
	// Test 4-Convert virtual to physical: tstpg v2p
	else if(strcmp(arguments[1], "v2p") == 0)
	{
		test_virtual_to_physical();
	}
	return 0;
}

int command_test_kheap(int number_of_arguments, char **arguments)
{
	// Parameters Validation Checking
	if (strcmp(arguments[2], "kmalloc") == 0 && number_of_arguments != 4)
	{
		cprintf("Invalid number of arguments! USAGE: tstkheap <Strategy> kmalloc <1 or 2 or 3>\n") ;
		return 0;
	}
	if (strcmp(arguments[2], "kmalloc") != 0 && number_of_arguments != 3)
	{
		cprintf("Invalid number of arguments! USAGE: tstkheap <Strategy> <testname>\n") ;
		return 0;
	}

	// Setting Strategy
	if(strcmp(arguments[1], "FF") == 0 || strcmp(arguments[1], "ff") == 0)
	{
		setKHeapPlacementStrategyFIRSTFIT();
		cprintf("Kernel Heap placement strategy is FIRST FIT\n");
	}
	else if(strcmp(arguments[1], "BF") == 0 || strcmp(arguments[1], "bf") == 0)
	{
		setKHeapPlacementStrategyBESTFIT();
		cprintf("Kernel Heap placement strategy is BEST FIT\n");
	}
	else if(strcmp(arguments[1], "NF") == 0 || strcmp(arguments[1], "nf") == 0)
	{
		setKHeapPlacementStrategyNEXTFIT();
		cprintf("Kernel Heap placement strategy is NEXT FIT\n");
	}

	// Test 1-kmalloc: tstkheap FF kmalloc 1
	if(strcmp(arguments[2], "kmalloc") == 0)
	{
		uint32 testNum = strtol(arguments[3], NULL, 10);
		if(isKHeapPlacementStrategyFIRSTFIT())
		{
			if (testNum == 0)
			{
				cprintf("Error: [Kernel.FirstFit] must specify the test number (1 or 2) as an argument\n");
				return 0;
			}
			//Test FIRST FIT allocation
			if (testNum == 1)
				test_kmalloc();
			else if (testNum == 2)
				test_kmalloc_firstfit1();
			else if (testNum == 3)
				test_kmalloc_firstfit2();
		}
		else if(isKHeapPlacementStrategyBESTFIT())
		{
			if (testNum == 0)
			{
				cprintf("Error: [Kernel.BestFit] must specify the test number (1, 2, 3) as an argument\n");
				return 0;
			}
			if (testNum == 1)
				test_kmalloc();
			else if (testNum == 2)
				test_kmalloc_bestfit1();
			else if (testNum == 3)
				test_kmalloc_bestfit2();
		}
		else if(isKHeapPlacementStrategyNEXTFIT())
		{
			if (testNum == 0)
			{
				cprintf("Error: [Kernel.NextFit] must specify the test number (1 or 2) as an argument\n");
				return 0;
			}
			//Test cont. allocation
			if (testNum == 1)
				test_kmalloc();
			//Test nextfit strategy
			else if (testNum == 2)
				test_kmalloc_nextfit();
		}
		return 0;
	}
	// Test 2-kfree: tstkheap FF kfree
	else if(strcmp(arguments[2], "kfree") == 0)
	{
		if (isKHeapPlacementStrategyBESTFIT() || isKHeapPlacementStrategyFIRSTFIT())
		{
			test_kfree_bestfirstfit();
		}
		else //NEXT & CONT
		{
			test_kfree();
		}
		return 0;
	}
	// Test 3-kphysaddr: tstkheap FF kphysaddr
	else if(strcmp(arguments[2], "kphysaddr") == 0)
	{
		test_kheap_phys_addr();
		return 0;
	}
	// Test 4-kvirtaddr: tstkheap FF kvirtaddr
	else if(strcmp(arguments[2], "kvirtaddr") == 0)
	{
		test_kheap_virt_addr();
		return 0;
	}
	// Test 5-krealloc: tstkheap BF krealloc
	else if(strcmp(arguments[2], "krealloc") == 0)
	{
		if (isKHeapPlacementStrategyBESTFIT())
		{
			test_krealloc_BF();
		}
		return 0;
	}
	return 0;
}
//END======================================================
