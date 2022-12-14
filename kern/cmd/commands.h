/*
 * commands.h
 *
 *  Created on: Oct 14, 2022
 *      Author: HP
 */

#ifndef KERN_CMD_COMMANDS_H_
#define KERN_CMD_COMMANDS_H_
#ifndef FOS_KERNEL
# error "This is a FOS kernel header; user programs should not #include it"
#endif
#include <inc/types.h>

//Structure for each command
struct Command
{
	char *name;
	char *description;
	// return -1 to force command prompt to exit
	int (*function_to_execute)(int number_of_arguments, char** arguments);
};

//Array of commands
extern struct Command commands[] ;
extern uint32 NUM_OF_COMMANDS ;

//=================================================================//
// Declaration of functions that implement command prompt commands.//
//=================================================================//
int command_help(int , char **);
int command_kernel_info(int , char **);

//Lab2.Hands.On
//=============
//TODO: Lab2.Hands.On: declare the command function here


//LAB3.Examples
//=============
int command_kernel_base_info(int , char **);
int command_del_kernel_base(int , char **);
int command_share_page(int , char **);

//Lab4.Hands.On
//=============
int command_show_mapping(int number_of_arguments, char **arguments);
int command_set_permission(int number_of_arguments, char **arguments);
int command_share_range(int number_of_arguments, char **arguments);

//Lab5.Examples
//=============
int command_nr(int number_of_arguments, char **arguments);
int command_ap(int , char **);
int command_fp(int , char **);

//Lab5.Hands-on
//=============
int command_asp(int, char **);
int command_cfp(int, char **);

//Lab6.Examples
//=============
int command_run(int , char **);
int command_kill(int , char **);
int command_ft(int , char **);

int command_calc_space(int number_of_arguments, char **arguments);
int command_run_program(int argc, char **argv);
int command_readmem_k(int number_of_arguments, char **arguments);
int command_writemem_k(int number_of_arguments, char **arguments);
int command_kill_program(int number_of_arguments, char **arguments);

int commnad_load_env(int number_of_arguments, char **arguments);
int command_run_all(int number_of_arguments, char **arguments);
int command_print_all(int number_of_arguments, char **arguments);
int command_kill_all(int number_of_arguments, char **arguments);

int command_writeusermem(int number_of_arguments, char **arguments);
int command_readusermem(int number_of_arguments, char **arguments);
int command_readuserblock(int number_of_arguments, char **arguments);
int command_remove_table(int number_of_arguments, char **arguments);
int command_allocuserpage(int number_of_arguments, char **arguments);
int command_meminfo(int number_of_arguments, char **arguments);

int command_set_page_rep_FIFO(int number_of_arguments, char **arguments);
int command_set_page_rep_CLOCK(int number_of_arguments, char **arguments);
int command_set_page_rep_LRU(int number_of_arguments, char **arguments);
int command_set_page_rep_ModifiedCLOCK(int number_of_arguments, char **arguments);
int command_set_page_rep_nthCLOCK(int number_of_arguments, char **arguments);
int command_print_page_rep(int number_of_arguments, char **arguments);

int command_set_uheap_plac_FIRSTFIT(int number_of_arguments, char **arguments);
int command_set_uheap_plac_BESTFIT(int number_of_arguments, char **arguments);
int command_set_uheap_plac_NEXTFIT(int number_of_arguments, char **arguments);
int command_set_uheap_plac_WORSTFIT(int number_of_arguments, char **arguments);
int command_print_uheap_plac(int number_of_arguments, char **arguments);

int command_set_kheap_plac_CONTALLOC(int number_of_arguments, char **arguments);
int command_set_kheap_plac_FIRSTFIT(int number_of_arguments, char **arguments);
int command_set_kheap_plac_BESTFIT(int number_of_arguments, char **arguments);
int command_set_kheap_plac_NEXTFIT(int number_of_arguments, char **arguments);
int command_set_kheap_plac_WORSTFIT(int number_of_arguments, char **arguments);
int command_print_kheap_plac(int number_of_arguments, char **arguments);

int command_disable_modified_buffer(int number_of_arguments, char **arguments);
int command_enable_modified_buffer(int number_of_arguments, char **arguments);

//2016
int command_disable_buffering(int number_of_arguments, char **arguments);
int command_enable_buffering(int number_of_arguments, char **arguments);

int command_set_modified_buffer_length(int number_of_arguments, char **arguments);
int command_get_modified_buffer_length(int number_of_arguments, char **arguments);

int command_test_kmalloc(int number_of_arguments, char **arguments);
int command_test_kfree(int number_of_arguments, char **arguments);
int command_test_kheap_phys_addr(int number_of_arguments, char **arguments);
int command_test_kheap_virt_addr(int number_of_arguments, char **arguments);
int command_test_three_creation_functions(int number_of_arguments, char **arguments);
int command_test_kfreeall(int number_of_arguments, char **arguments);
int command_test_kexpand(int number_of_arguments, char **arguments);
int command_test_kshrink(int number_of_arguments, char **arguments);
int command_test_kfreelast(int number_of_arguments, char **arguments);
int command_test_krealloc(int number_of_arguments, char **arguments);
int command_test_sc4_MLFQ(int number_of_arguments, char **arguments);

//2018
int command_sch_RR(int number_of_arguments, char **arguments);
int command_sch_MLFQ(int number_of_arguments, char **arguments);
int command_print_sch_method(int number_of_arguments, char **arguments);
int command_sch_test(int number_of_arguments, char **arguments);
int command_test_sc_MLFQ(int number_of_arguments, char **arguments);

int command_test_priority1(int number_of_arguments, char **arguments);
int command_test_priority2(int number_of_arguments, char **arguments);

//2022
int command_test_autocomplete(int number_of_arguments, char **arguments);
int command_test_lists(int number_of_arguments, char **arguments);
int command_test_paging_manipulation(int number_of_arguments, char **arguments);
int command_test_chunks(int number_of_arguments, char **arguments);
int command_test_kheap(int number_of_arguments, char **arguments);

#endif /* KERN_CMD_COMMANDS_H_ */
