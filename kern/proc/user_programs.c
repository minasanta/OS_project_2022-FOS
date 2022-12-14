/*
 * user_programs.c
 *
 *  Created on: Oct 12, 2022
 *      Author: HP
 */
#include <kern/proc/user_environment.h>
#include <inc/string.h>
#include <inc/assert.h>



//User Programs Table
//The input for any PTR_START_OF macro must be the ".c" filename of the user program
struct UserProgramInfo userPrograms[] = {
		/*PROGRAMS*/
		{ "fos_helloWorld", "Created by FOS team, fos@nowhere.com", PTR_START_OF(fos_helloWorld)},
		{ "fos_add", "Created by FOS team, fos@nowhere.com", PTR_START_OF(fos_add)},
		{ "fos_alloc", "Created by FOS team, fos@nowhere.com", PTR_START_OF(fos_alloc)},
		{ "fos_input", "Created by FOS team, fos@nowhere.com", PTR_START_OF(fos_input)},
		{ "fos_game", "Created by FOS team, fos@nowhere.com", PTR_START_OF(game)},
		{ "fos_static_data_section", "Created by FOS team, fos@nowhere.com", PTR_START_OF(fos_static_data_section)},
		{ "fos_data_on_stack", "Created by FOS team, fos@nowhere.com", PTR_START_OF(fos_data_on_stack)},
		{ "tqsfh", "Quicksort with freeHeap", PTR_START_OF(tst_quicksort_freeHeap)},
		{ "fact", "Factorial Recursive", PTR_START_OF(fos_factorial)},
		{ "fib", "Fibonacci Recursive", PTR_START_OF(fos_fibonacci)},
		{ "tqs", "Quicksort that cause memory leakage", PTR_START_OF(quicksort_heap)},
		{ "qs", "Quicksort with NO memory leakage", PTR_START_OF(quicksort_noleakage)},
		{ "ms1", "Mergesort with NO memory leakage", PTR_START_OF(mergesort_noleakage)},
		{ "ms2", "Mergesort that cause memory leakage", PTR_START_OF(mergesort_leakage)},

		/*PAGE FAULT HANDLER*/
		//PLACEMENT
		{ "tpp", "Tests the Page placement", PTR_START_OF(tst_placement)},
		{ "tia", "tests handling of invalid memory access", PTR_START_OF(tst_invalid_access)},
		//REPLACEMENT
		{ "tpr1", "Tests page replacement (allocation of Memory and PageFile)", PTR_START_OF(tst_page_replacement_alloc)},
		{ "tpr2", "tests page replacement (handling new stack and modified pages)", PTR_START_OF(tst_page_replacement_stack)},
		{ "tclock", "Tests page replacement (clock algorithm)", PTR_START_OF(tst_page_replacement_clock)},

		/*USER HEAP*/
		//ALLOC USING LARGE SIZES
		/*TESTING 2022*/
		{ "tm0", "tests malloc (0): check initialize_dyn_block_system in UHEAP", PTR_START_OF(tst_malloc_0)},
		/***/
		{ "tm1", "tests malloc (1): start address & allocated frames", PTR_START_OF(tst_malloc_1)},
		{ "tm2", "tests malloc (2): writing & reading values in allocated spaces", PTR_START_OF(tst_malloc_2)},
		{ "tm3", "tests malloc (3): check memory allocation and WS after accessing", PTR_START_OF(tst_malloc_3)},
		//DEALLOCATION USING LARGE SIZES
		{ "tf1", "tests free (1): freeing tables, WS and page file [placement case]", PTR_START_OF(tst_free_1)},
		{ "tf2", "tests free (2): try accessing values in freed spaces", PTR_START_OF(tst_free_2)},
		{ "hp", "heap program (allocate and free from heap) + placement after free", PTR_START_OF(heap_program)},
		//STRATEGIES for LARGE SIZES
		{ "tff1", "tests first fit (1): always find suitable space", PTR_START_OF(tst_first_fit_1)},
		{ "tff2", "tests first fit (2): no suitable space", PTR_START_OF(tst_first_fit_2)},
		{ "tnf", "tests next fit: all cases", PTR_START_OF(tst_nextfit)},
		{ "tbf1", "tests best fit (1): always find suitable space", PTR_START_OF(tst_best_fit_1)},
		{ "tbf2", "tests best fit (2): no suitable space", PTR_START_OF(tst_best_fit_2)},
		{ "twf", "tests worst fit: all cases", PTR_START_OF(tst_worstfit)},
		//SHARING
		{ "tff3", "tests first fit (3): malloc, smalloc & sget", PTR_START_OF(tst_first_fit_3)},
		{ "tshr1", "Tests the shared variables [create]", PTR_START_OF(tst_sharing_1)},
		{ "tshr2", "Tests the shared variables [create, get and perms]", PTR_START_OF(tst_sharing_2master)},
		{ "shr2Slave1", "[Slave program1] of tst_sharing_2master", PTR_START_OF(tst_sharing_2slave1)},
		{ "shr2Slave2", "[Slave program2] of tst_sharing_2master", PTR_START_OF(tst_sharing_2slave2)},
		{ "tshr3", "Tests the shared variables [Special cases of create]", PTR_START_OF(tst_sharing_3)},
		{ "tshr4", "Tests the free of shared variables after createSharedObject only", PTR_START_OF(tst_sharing_4)},
		{ "tshr5", "Tests the free of shared variables after both createSharedObject and getSharedObject", PTR_START_OF(tst_sharing_5_master)},
		{ "tshr5slave", "Slave program to be used with tshr5", PTR_START_OF(tst_sharing_5_slave)},
		{ "tshr5slaveB1", "Slave program to be used with tshr5", PTR_START_OF(tst_sharing_5_slaveB1)},
		{ "tshr5slaveB2", "Slave program to be used with tshr5", PTR_START_OF(tst_sharing_5_slaveB2)},
		//REALLOC
		{ "tr1", "tests realloc (1): normal cases", PTR_START_OF(tst_realloc_1)},
		{ "tr2", "tests realloc (2): special cases", PTR_START_OF(tst_realloc_2)},
		{ "tr3", "tests realloc (3): content when realloc to other location", PTR_START_OF(tst_realloc_3)},

		/*SEMAPHORES*/
		{ "tsem1", "Tests the Semaphores only [critical section & dependency]", PTR_START_OF(tst_semaphore_1master)},
		{ "sem1Slave", "[Slave program] of tst_semaphore_1master", PTR_START_OF(tst_semaphore_1slave)},
		{ "tsem2", "Tests the Semaphores only [multiprograms enter the same CS]", PTR_START_OF(tst_semaphore_2master)},
		{ "sem2Slave", "[Slave program] of tst_semaphore_2master", PTR_START_OF(tst_semaphore_2slave)},

		/*SCENARIOS*/
		{ "arrop", "Apply set of array operations: scenario program to test shared objects", PTR_START_OF(arrayOperations_Master)},
		{ "slave_qs", "SlaveOperation: quicksort", PTR_START_OF(arrayOperations_quicksort)},
		{ "slave_ms", "SlaveOperation: mergesort", PTR_START_OF(arrayOperations_mergesort)},
		{ "slave_stats", "SlaveOperation: stats", PTR_START_OF(arrayOperations_stats)},

		{ "tair", "", PTR_START_OF(tst_air)},
		{ "taircl", "", PTR_START_OF(tst_air_clerk)},
		{ "taircu", "", PTR_START_OF(tst_air_customer)},

		{ "midterm", "Midterm 2017: Example on shared resource and dependency", PTR_START_OF(MidTermEx_Master)},
		{ "midterm_a", "Midterm 2017 Example: Process A", PTR_START_OF(MidTermEx_ProcessA)},
		{ "midterm_b", "Midterm 2017 Example: Process B", PTR_START_OF(MidTermEx_ProcessB)},

		/*EXIT ENVIRONMENT*/
		{ "ef_fib", "", PTR_START_OF(ef_fos_fibonacci)},
		{ "ef_fact", "", PTR_START_OF(ef_fos_factorial)},
		{ "ef_fos_add", "", PTR_START_OF(ef_fos_add)},
		{ "tef1", "", PTR_START_OF(tst_envfree1)},
		{ "ef_ms1", "", PTR_START_OF(ef_mergesort_noleakage)},
		{ "ef_ms2", "", PTR_START_OF(ef_mergesort_leakage)},
		{ "tef2", "", PTR_START_OF(tst_envfree2)},
		{ "ef_tshr1", "", PTR_START_OF(ef_tst_sharing_1)},
		{ "ef_tshr2", "", PTR_START_OF(ef_tst_sharing_2master)},
		{ "ef_shr2Slave1", "", PTR_START_OF(ef_tst_sharing_2slave1)},
		{ "tef3", "", PTR_START_OF(tst_envfree3)},
		{ "ef_tsem1", "", PTR_START_OF(ef_tst_semaphore_1master)},
		{ "ef_sem1Slave", "", PTR_START_OF(ef_tst_semaphore_1slave)},
		{ "tef4", "", PTR_START_OF(tst_envfree4)},
		{ "ef_tshr4", "", PTR_START_OF(ef_tst_sharing_4)},
		{ "tef5_1", "", PTR_START_OF(tst_envfree5_1)},
		{ "ef_tshr4", "", PTR_START_OF(ef_tst_sharing_4)},
		{ "ef_tshr5", "", PTR_START_OF(ef_tst_sharing_5_master)},
		{ "ef_tshr5slave", "", PTR_START_OF(ef_tst_sharing_5_slave)},
		{ "ef_tshr5slaveB1", "", PTR_START_OF(ef_tst_sharing_5_slaveB1)},
		{ "ef_tshr5slaveB2", "", PTR_START_OF(ef_tst_sharing_5_slaveB2)},
		{ "tef5_2", "", PTR_START_OF(tst_envfree5_2)},
		{ "ef_tshr1", "", PTR_START_OF(ef_tst_sharing_1)},
		{ "ef_midterm", "", PTR_START_OF(ef_MidTermEx_Master)},
		{ "tef6", "", PTR_START_OF(tst_envfree6)},

		/*OTHERS*/
		{ "tfr", "tests freeRAM (1): run in specific scenario", PTR_START_OF(tst_freeRAM)},
		//{ "tfr2", "tests freeRAM (2): run directly", PTR_START_OF(tst_freeRAM_2)},
		{ "tfs", "Tests freeing the unused stack pages", PTR_START_OF(tst_freeing_stack)},

};

///=========================================================

// To be used as extern in other files
struct UserProgramInfo* ptr_UserPrograms = &userPrograms[0];

// Number of user programs in the program table
int NUM_USER_PROGS = (sizeof(userPrograms)/sizeof(userPrograms[0]));

struct UserProgramInfo* get_user_program_info(char* user_program_name)
{
	int i;
	for (i = 0; i < NUM_USER_PROGS; i++) {
		if (strcmp(user_program_name, userPrograms[i].name) == 0)
			break;
	}
	if(i==NUM_USER_PROGS)
	{
		cprintf("Unknown user program '%s'\n", user_program_name);
		return 0;
	}

	return &userPrograms[i];
}

struct UserProgramInfo* get_user_program_info_by_env(struct Env* e)
{
	int i;
	for (i = 0; i < NUM_USER_PROGS; i++) {
		if ( strcmp( e->prog_name , userPrograms[i].name) ==0)
			break;
	}
	if(i==NUM_USER_PROGS)
	{
		cprintf("Unknown user program \n");
		return 0;
	}

	return &userPrograms[i];
}
