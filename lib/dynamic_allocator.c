/*
 * dyn_block_management.c
 *
 *  Created on: Sep 21, 2022
 *      Author: HP
 */
#include <inc/assert.h>
#include <inc/string.h>
#include "../inc/dynamic_allocator.h"


//==================================================================================//
//============================== GIVEN FUNCTIONS ===================================//
//==================================================================================//

//===========================
// PRINT MEM BLOCK LISTS:
//===========================

void print_mem_block_lists()
{
	cprintf("\n=========================================\n");
	struct MemBlock* blk ;
	struct MemBlock* lastBlk = NULL ;
	cprintf("\nFreeMemBlocksList:\n");
	uint8 sorted = 1 ;
	LIST_FOREACH(blk, &FreeMemBlocksList)
	{
		if (lastBlk && blk->sva < lastBlk->sva + lastBlk->size)
			sorted = 0 ;
		cprintf("[%x, %x)-->", blk->sva, blk->sva + blk->size) ;
		lastBlk = blk;
	}
	if (!sorted)	cprintf("\nFreeMemBlocksList is NOT SORTED!!\n") ;

	lastBlk = NULL ;
	cprintf("\nAllocMemBlocksList:\n");
	sorted = 1 ;
	LIST_FOREACH(blk, &AllocMemBlocksList)
	{
		if (lastBlk && blk->sva < lastBlk->sva + lastBlk->size)
			sorted = 0 ;
		cprintf("[%x, %x)-->", blk->sva, blk->sva + blk->size) ;
		lastBlk = blk;
	}
	if (!sorted)	cprintf("\nAllocMemBlocksList is NOT SORTED!!\n") ;
	cprintf("\n=========================================\n");

}

//********************************************************************************//
//********************************************************************************//

//==================================================================================//
//============================ REQUIRED FUNCTIONS ==================================//
//==================================================================================//

//===============================
// [1] INITIALIZE AVAILABLE LIST:
//===============================
void initialize_MemBlocksList(uint32 numOfBlocks)
{
	//TODO: [PROJECT MS1] [DYNAMIC ALLOCATOR] initialize_MemBlocksList
	// Write your code here, remove the panic and write your code
	// panic("initialize_MemBlocksList() is not implemented yet...!!");
	LIST_INIT(&AvailableMemBlocksList);
	for(int i = 0; i < numOfBlocks; i++) {
		LIST_INSERT_HEAD(&AvailableMemBlocksList , &(MemBlockNodes[i]));
	}
}

//===============================
// [2] FIND BLOCK:
//===============================
struct MemBlock *find_block(struct MemBlock_List *blockList, uint32 va)
{
	//TODO: [PROJECT MS1] [DYNAMIC ALLOCATOR] find_block
	// Write your code here, remove the panic and write your code
	// panic("find_block() is not implemented yet...!!");
	struct MemBlock *element;
	LIST_FOREACH(element, *(&blockList))
	{
		if(element->sva == va) {
			return element;
		}
	}
	return NULL;
}

//=========================================
// [3] INSERT BLOCK IN ALLOC LIST [SORTED]:
//=========================================
void insert_sorted_allocList(struct MemBlock *blockToInsert)
{
	//TODO: [PROJECT MS1] [DYNAMIC ALLOCATOR] insert_sorted_allocList
		// Write your code here, remove the panic and write your code
	   // panic("insert_sorted_allocList() is not implemented yet...!!");
		int size = LIST_SIZE(&AllocMemBlocksList);
		if (size == 0 )
		{
			LIST_INSERT_HEAD((&AllocMemBlocksList),blockToInsert);

		}

		else
		{
			struct MemBlock *iterator;
			struct MemBlock *compare;
			bool found = 0;
			LIST_FOREACH(iterator,(&AllocMemBlocksList))
			{
			 if(iterator->sva > blockToInsert->sva )
		        {
				 compare=iterator;
				 found = 1;
				 break;
		        }
			}
	     	struct MemBlock* Lelement = LIST_LAST(&AllocMemBlocksList);
	     	struct MemBlock* Felement = LIST_FIRST(&AllocMemBlocksList);

			if (found == 0)
			{
				LIST_INSERT_TAIL((&AllocMemBlocksList),blockToInsert);
			}

			else if(compare->sva == Felement->sva){
				LIST_INSERT_HEAD((&AllocMemBlocksList),blockToInsert);
			}

			else
			{
				struct MemBlock* prev = LIST_PREV(compare);
				blockToInsert->prev_next_info.le_next=compare;
				blockToInsert->prev_next_info.le_prev=compare->prev_next_info.le_prev;
				prev->prev_next_info.le_next=blockToInsert;
				compare->prev_next_info.le_prev=blockToInsert;
				AllocMemBlocksList.size++;


			}

		}
}

//=========================================
// [4] ALLOCATE BLOCK BY FIRST FIT:
//=========================================
struct MemBlock *alloc_block_FF(uint32 size)
{
	// TODO: [PROJECT MS1] [DYNAMIC ALLOCATOR] alloc_block_FF
	// Write your code here, remove the panic and write your code
	// panic("alloc_block_FF() is not implemented yet...!!");
	struct MemBlock *tmp;

		LIST_FOREACH(tmp, &FreeMemBlocksList)
		{
			if (tmp->size == size) {
				struct MemBlock *found = tmp;

				LIST_REMOVE(&FreeMemBlocksList, found);

				return found;
			} else if (tmp->size > size) {
				struct MemBlock *new = LIST_FIRST(&AvailableMemBlocksList);
				new->size = size;
				new->sva = tmp->sva;

				LIST_REMOVE(&AvailableMemBlocksList, new);

				tmp->size -= size;
				tmp->sva += size;

				return new;
			}

		}
		return NULL;
}

//=========================================
// [5] ALLOCATE BLOCK BY BEST FIT:
//=========================================
struct MemBlock *alloc_block_BF(uint32 size)
{
	//TODO: [PROJECT MS1] [DYNAMIC ALLOCATOR] alloc_block_BF
	// Write your code here, remove the panic and write your code
	//panic("alloc_block_BF() is not implemented yet...!!");
	struct MemBlock *tmp, *min_tmp;
		uint32 min = 4294967295;

		LIST_FOREACH (tmp, &FreeMemBlocksList)
		{
			if (tmp->size == size) {
				struct MemBlock *found = tmp;

				LIST_REMOVE(&FreeMemBlocksList, found);

				return found;
			} else if (tmp->size > size) {
				uint32 dif = tmp->size - size;
				if (dif < min) {
					min = dif;
					min_tmp = tmp;
				}

			}
		}

		if (min == 4294967295) {
			return NULL;
		} else {
			struct MemBlock *new = LIST_FIRST(&AvailableMemBlocksList);
			new->size = size;
			new->sva = min_tmp->sva;

			LIST_REMOVE(&AvailableMemBlocksList, new);

			min_tmp->size -= size;
			min_tmp->sva += size;

			return new;

		}
}


//=========================================
// [7] ALLOCATE BLOCK BY NEXT FIT:
//=========================================
struct MemBlock *stopped = NULL;
struct MemBlock *alloc_block_NF(uint32 size) {
	//TODO: [PROJECT MS1 - BONUS] [DYNAMIC ALLOCATOR] alloc_block_NF
	// Write your code here, remove the panic and write your code
	//	panic("Congratiolations!!");

	struct MemBlock *tmp;

	if (stopped != NULL)
	{

		tmp = stopped;

		if (LIST_NEXT(tmp) != LIST_LAST(&FreeMemBlocksList)) {

			while (LIST_NEXT(tmp)->sva != 0) {

				if (tmp->size == size) {

					struct MemBlock *found = tmp;

					if (LIST_NEXT(found) != NULL)
						stopped = LIST_NEXT(found);
					else
						stopped = NULL;

					LIST_REMOVE(&FreeMemBlocksList, found);

					return found;

				} else if (tmp->size > size) {

					struct MemBlock *new = LIST_FIRST(&AvailableMemBlocksList);

					new->size = size;
					new->sva = tmp->sva;

					if (LIST_NEXT(tmp) != NULL)
						stopped = tmp;
					else
						stopped = NULL;

					LIST_REMOVE(&AvailableMemBlocksList, new);

					tmp->size -= size;
					tmp->sva += size;

					return new;
				}
				tmp = LIST_NEXT(tmp);
			}
		} //that means it is the last item in the list
		else {
			if (tmp->size == size) {

				struct MemBlock *found = tmp;

					stopped = NULL;

				LIST_REMOVE(&FreeMemBlocksList, found);

				return found;

			} else if (tmp->size > size) {

				struct MemBlock *new = LIST_FIRST(&AvailableMemBlocksList);

				new->size = size;
				new->sva = tmp->sva;

				stopped = NULL;

				LIST_REMOVE(&AvailableMemBlocksList, new);

				tmp->size -= size;
				tmp->sva += size;

				return new;
			}
		}
	}

	tmp = NULL;
	LIST_FOREACH (tmp, &FreeMemBlocksList)
	{
		if (tmp->size == size) {
			struct MemBlock *found = tmp;

			if (LIST_NEXT(found) != NULL)
				stopped = LIST_NEXT(found);
			else
				stopped = NULL;

			LIST_REMOVE(&FreeMemBlocksList, tmp);

			return found;
		} else if (tmp->size > size) {
			struct MemBlock *new = LIST_FIRST(&AvailableMemBlocksList);

			new->size = size;
			new->sva = tmp->sva;

			if (LIST_NEXT(tmp) != NULL)
				stopped = tmp;
			else
				stopped = NULL;

			LIST_REMOVE(&AvailableMemBlocksList, new);

			tmp->size -= size;
			tmp->sva += size;

			return new;
		}

	}
	return NULL;
}


//===================================================
// [8] INSERT BLOCK (SORTED WITH MERGE) IN FREE LIST:
//===================================================
void insert_sorted_with_merge_freeList(struct MemBlock *blockToInsert)
{
	//cprintf("BEFORE INSERT with MERGE: insert [%x, %x)\n=====================\n", blockToInsert->sva, blockToInsert->sva + blockToInsert->size);
	//print_mem_block_lists() ;

	//TODO: [PROJECT MS1] [DYNAMIC ALLOCATOR] insert_sorted_with_merge_freeList
	// Write your code here, remove the panic and write your code
	int sizeOfFree = LIST_SIZE(&FreeMemBlocksList);
	if (sizeOfFree == 0)
	{
		LIST_INSERT_HEAD(&FreeMemBlocksList, blockToInsert);
	}
	else
	{
		struct MemBlock *element;
		int firstPass = 0;
		LIST_FOREACH (element, &FreeMemBlocksList)
		{
			if (firstPass == 1 && element->sva > blockToInsert->sva)
			{
				struct MemBlock *prev = LIST_PREV(element);
				uint32 addressPrev = prev->size + prev->sva;
				uint32 addressInsertedBlock = blockToInsert->size + blockToInsert->sva;
				if(addressPrev == blockToInsert->sva && addressInsertedBlock == element->sva)
				{
					prev->size += (blockToInsert->size + element->size);
					blockToInsert->size = 0;
					blockToInsert->sva = 0;
					element->size = 0;
					element->sva = 0;
					LIST_REMOVE(&FreeMemBlocksList, element);
					LIST_INSERT_HEAD(&AvailableMemBlocksList, blockToInsert);
					LIST_INSERT_AFTER(&AvailableMemBlocksList, blockToInsert, element);
					return;
				}
				else if (addressPrev == blockToInsert->sva)
				{
					prev->size += blockToInsert->size;
					blockToInsert->size = 0;
					blockToInsert->sva = 0;
					LIST_INSERT_HEAD(&AvailableMemBlocksList, blockToInsert);
					return;
				}
				else if (addressInsertedBlock == element->sva)
				{
					element->size += blockToInsert->size;
					element->sva = blockToInsert->sva;
					blockToInsert->size = 0;
					blockToInsert->sva = 0;
					LIST_INSERT_HEAD(&AvailableMemBlocksList, blockToInsert);
					return;
				}
				else
				{
					LIST_INSERT_BEFORE(&FreeMemBlocksList, element, blockToInsert);
					return;
				}
			}
			if (firstPass == 0 && element->sva > blockToInsert->sva)
			{
				uint32 addressInsertedBlock = blockToInsert->size + blockToInsert->sva;
				if (addressInsertedBlock == element->sva)
				{
					element->sva = blockToInsert->sva;
					element->size += blockToInsert->size;
					blockToInsert->sva = 0;
					blockToInsert->size = 0;
					LIST_INSERT_HEAD(&AvailableMemBlocksList, blockToInsert);
					return;
				}
				LIST_INSERT_HEAD(&FreeMemBlocksList, blockToInsert);
				return;
			}
			firstPass = 1;
		}
		element = LIST_LAST(&FreeMemBlocksList);
		uint32 addressLast = element->sva + element->size;
		if (addressLast == blockToInsert->sva) {
			element->size += blockToInsert->size;
			blockToInsert->size = 0;
			blockToInsert->sva = 0;
			LIST_INSERT_HEAD(&AvailableMemBlocksList, blockToInsert);
		}
		else {
			LIST_INSERT_TAIL(&FreeMemBlocksList, blockToInsert);
		}
	}

	//cprintf("\nAFTER INSERT with MERGE:\n=====================\n");
	//print_mem_block_lists();

}

