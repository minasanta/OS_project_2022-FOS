#ifndef FOS_INC_DYNBLK_MANAGE_H
#define FOS_INC_DYNBLK_MANAGE_H
#include <inc/queue.h>
#include <inc/types.h>

/*Data*/

#define STATIC_MEMBLOCK_ALLOC 0

typedef LIST_ENTRY(MemBlock) MemBlock_LIST_entry_t;
LIST_HEAD(MemBlock_List, MemBlock); //macro define struct named with MemBlock_List, and the type of the its pointers

struct MemBlock
{
	MemBlock_LIST_entry_t prev_next_info;	/* list link */
	uint32 sva ; 	//start virtual address
	uint32 size;	//block size
};

#if STATIC_MEMBLOCK_ALLOC
	#define MAX_MEM_BLOCK_CNT 10000
	struct MemBlock MemBlockNodes[MAX_MEM_BLOCK_CNT] ;
#else
	uint32 MAX_MEM_BLOCK_CNT;
	struct MemBlock * MemBlockNodes;
#endif

//Dynamic MemBlock Lists
struct MemBlock_List AvailableMemBlocksList;
struct MemBlock_List FreeMemBlocksList;
struct MemBlock_List AllocMemBlocksList;

/*Functions*/
//PROJECT MS#1
void initialize_MemBlocksList(uint32 numOfBlocks);
void insert_sorted_allocList(struct MemBlock *blockToInsert);
struct MemBlock *find_block(struct MemBlock_List *blockList, uint32 va);
struct MemBlock *alloc_block_FF(uint32 size);
struct MemBlock *alloc_block_BF(uint32 size);
struct MemBlock *alloc_block_NF(uint32 size);
void insert_sorted_with_merge_freeList(struct MemBlock *blockToInsert) ;
//==========================================================

//PROJECT MS#2 & MS#3
//In KernelHeap: should be implemented inside kern/kheap.c
//In UserHeap: should be implemented inside lib/uheap.c
void initialize_dyn_block_system();
//==========================================================


void print_mem_block_lists();
#endif
