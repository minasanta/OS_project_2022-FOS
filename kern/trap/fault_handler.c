/*
 * fault_handler.c
 *
 *  Created on: Oct 12, 2022
 *      Author: HP
 */

#include "trap.h"
#include <kern/proc/user_environment.h>
#include "../cpu/sched.h"
#include "../disk/pagefile_manager.h"
#include "../mem/memory_manager.h"

//2014 Test Free(): Set it to bypass the PAGE FAULT on an instruction with this length and continue executing the next one
// 0 means don't bypass the PAGE FAULT
uint8 bypassInstrLength = 0;

//===============================
// REPLACEMENT STRATEGIES
//===============================
//2020
void setPageReplacmentAlgorithmLRU(int LRU_TYPE)
{
	assert(LRU_TYPE == PG_REP_LRU_TIME_APPROX || LRU_TYPE == PG_REP_LRU_LISTS_APPROX);
	_PageRepAlgoType = LRU_TYPE ;
}
void setPageReplacmentAlgorithmCLOCK(){_PageRepAlgoType = PG_REP_CLOCK;}
void setPageReplacmentAlgorithmFIFO(){_PageRepAlgoType = PG_REP_FIFO;}
void setPageReplacmentAlgorithmModifiedCLOCK(){_PageRepAlgoType = PG_REP_MODIFIEDCLOCK;}
/*2018*/ void setPageReplacmentAlgorithmDynamicLocal(){_PageRepAlgoType = PG_REP_DYNAMIC_LOCAL;}
/*2021*/ void setPageReplacmentAlgorithmNchanceCLOCK(int PageWSMaxSweeps){_PageRepAlgoType = PG_REP_NchanceCLOCK;  page_WS_max_sweeps = PageWSMaxSweeps;}

//2020
uint32 isPageReplacmentAlgorithmLRU(int LRU_TYPE){return _PageRepAlgoType == LRU_TYPE ? 1 : 0;}
uint32 isPageReplacmentAlgorithmCLOCK(){if(_PageRepAlgoType == PG_REP_CLOCK) return 1; return 0;}
uint32 isPageReplacmentAlgorithmFIFO(){if(_PageRepAlgoType == PG_REP_FIFO) return 1; return 0;}
uint32 isPageReplacmentAlgorithmModifiedCLOCK(){if(_PageRepAlgoType == PG_REP_MODIFIEDCLOCK) return 1; return 0;}
/*2018*/ uint32 isPageReplacmentAlgorithmDynamicLocal(){if(_PageRepAlgoType == PG_REP_DYNAMIC_LOCAL) return 1; return 0;}
/*2021*/ uint32 isPageReplacmentAlgorithmNchanceCLOCK(){if(_PageRepAlgoType == PG_REP_NchanceCLOCK) return 1; return 0;}

//===============================
// PAGE BUFFERING
//===============================
void enableModifiedBuffer(uint32 enableIt){_EnableModifiedBuffer = enableIt;}
uint8 isModifiedBufferEnabled(){  return _EnableModifiedBuffer ; }

void enableBuffering(uint32 enableIt){_EnableBuffering = enableIt;}
uint8 isBufferingEnabled(){  return _EnableBuffering ; }

void setModifiedBufferLength(uint32 length) { _ModifiedBufferLength = length;}
uint32 getModifiedBufferLength() { return _ModifiedBufferLength;}

//===============================
// FAULT HANDLERS
//===============================

//Handle the table fault
void table_fault_handler(struct Env * curenv, uint32 fault_va)
{
	//panic("table_fault_handler() is not implemented yet...!!");
	//Check if it's a stack page
	uint32* ptr_table;
#if USE_KHEAP
	{
		ptr_table = create_page_table(curenv->env_page_directory, (uint32)fault_va);
	}
#else
	{
		__static_cpt(curenv->env_page_directory, (uint32)fault_va, &ptr_table);
	}
#endif
}

//Handle the page fault
bool replacement(uint32 start, uint32 end, uint32 fault_va ,struct Env * curenv)
{

	for(uint32 i = start; i < end;i++)
	{
		uint32 *ptr_page_table = NULL;
		uint32 permMOD, permUS;
		int PageTable = get_page_table(curenv->env_page_directory,curenv->ptr_pageWorkingSet[i].virtual_address,&ptr_page_table);
		uint32 table_entry = ptr_page_table[(PTX(curenv->ptr_pageWorkingSet[i].virtual_address))];
		permMOD = table_entry & PERM_MODIFIED;
		permUS = table_entry & PERM_USED;

		if (permUS)
			pt_set_page_permissions(curenv->env_page_directory,curenv->ptr_pageWorkingSet[i].virtual_address, 0,PERM_USED);

		else
		{
			if(permMOD)
			{
				struct FrameInfo *modified_frame =get_frame_info(curenv->env_page_directory,curenv->ptr_pageWorkingSet[i].virtual_address,&ptr_page_table);

				pf_update_env_page(curenv,curenv->ptr_pageWorkingSet[i].virtual_address, modified_frame);

			}

			unmap_frame(curenv->env_page_directory,curenv->ptr_pageWorkingSet[i].virtual_address);

			env_page_ws_clear_entry(curenv, i);

			struct FrameInfo *frame = NULL;
			allocate_frame(&frame);
			map_frame(curenv->env_page_directory, frame, fault_va,PERM_USER | PERM_WRITEABLE);
			frame->va = fault_va;

			int x = pf_read_env_page(curenv, (void *) fault_va);
			if (x == E_PAGE_NOT_EXIST_IN_PF)
			{
				// CHECK if it is a stack or heap page ==> ignore it
				if((fault_va >= USTACKBOTTOM && fault_va < USTACKTOP) ||(fault_va >= USER_HEAP_START && fault_va < USER_HEAP_MAX))
				{
					// It's OK <3
				}
				else
				{
					// Unmap
					unmap_frame(curenv->env_page_directory, fault_va);
					panic("ILLEGAL MEMORY ACCESS");
				}
			}

			env_page_ws_set_entry(curenv, i, fault_va);
			curenv->page_last_WS_index = i;
			return 1;
		}
	}
	return 0;
}

void page_fault_handler(struct Env * curenv, uint32 fault_va) {
	// TODO: [PROJECT MS3] [FAULT HANDLER] page_fault_handler
	// Write your code here, remove the panic and write your code
	// panic("page_fault_handler() is not implemented yet...!!");

	if (env_page_ws_get_size(curenv) < curenv->page_WS_max_size)
	{
		struct FrameInfo *ptr = NULL;
		allocate_frame(&ptr);
		map_frame(curenv->env_page_directory, ptr, fault_va,
		PERM_USER | PERM_WRITEABLE);
		ptr->va = fault_va;

		int x = pf_read_env_page(curenv, (void *) fault_va);
		if (x == E_PAGE_NOT_EXIST_IN_PF) {
			// CHECK if it is a stack or heap page ==> ignore it
			if ((fault_va >= USTACKBOTTOM && fault_va < USTACKTOP)
					|| (fault_va >= USER_HEAP_START && fault_va < USER_HEAP_MAX)) {
				// It's OK
			} else
				panic("ILLEGAL MEMORY ACCESS");
		}

		for (int i = 0; i < curenv->page_WS_max_size; i++)
			if (env_page_ws_is_entry_empty(curenv, i) == 1) {
				env_page_ws_set_entry(curenv, i, fault_va);
				break;
			}
	}
	else
	{ // replacement

		bool found = replacement(curenv->page_last_WS_index,curenv->page_WS_max_size, fault_va, curenv);
		curenv->page_last_WS_index++;
		curenv->page_last_WS_index = curenv->page_last_WS_index % curenv->page_WS_max_size;

		if(!found)
		{
			found = replacement(0,curenv->page_last_WS_index, fault_va, curenv);
			curenv->page_last_WS_index++;
			curenv->page_last_WS_index = curenv->page_last_WS_index % curenv->page_WS_max_size;

		}

		if(!found)
		{
			found = replacement(curenv->page_last_WS_index,curenv->page_last_WS_index+1, fault_va, curenv);
			curenv->page_last_WS_index++;
			curenv->page_last_WS_index = curenv->page_last_WS_index % curenv->page_WS_max_size;
		}

	}

	//refer to the project presentation and documentation for details
}

void __page_fault_handler_with_buffering(struct Env * curenv, uint32 fault_va)
{
	// Write your code here, remove the panic and write your code
	panic("__page_fault_handler_with_buffering() is not implemented yet...!!");


}
