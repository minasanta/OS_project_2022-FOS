#include "kheap.h"

#include <inc/memlayout.h>
#include <inc/dynamic_allocator.h>
#include "memory_manager.h"

//==================================================================//
//==================================================================//
//NOTE: All kernel heap allocations are multiples of PAGE_SIZE (4KB)//
//==================================================================//
//==================================================================//

void initialize_dyn_block_system() {
	//TODO: [PROJECT MS2] [KERNEL HEAP] initialize_dyn_block_system
	// your code is here, remove the panic and write your code
//	kpanic_into_prompt("initialize_dyn_block_system() is not implemented yet...!!");

	uint32 Kheap_size = KERNEL_HEAP_MAX - KERNEL_HEAP_START;
	uint32 MemBlock_size = ROUNDUP(NUM_OF_KHEAP_PAGES * sizeof(struct MemBlock),
			PAGE_SIZE);

	//[1] Initialize two lists (AllocMemBlocksList & FreeMemBlocksList) [Hint: use LIST_INIT()]
	LIST_INIT(&AllocMemBlocksList);
	LIST_INIT(&FreeMemBlocksList);

#if STATIC_MEMBLOCK_ALLOC
	//DO NOTHING
#else
	/*[2] Dynamically allocate the array of MemBlockNodes
	 * 	remember to:
	 * 		1. set MAX_MEM_BLOCK_CNT with the chosen size of the array
	 * 		2. allocation should be aligned on PAGE boundary
	 * 	HINT: can use alloc_chunk(...) function
	 */
	MAX_MEM_BLOCK_CNT = NUM_OF_KHEAP_PAGES;

	MemBlockNodes = (void*) KERNEL_HEAP_START;

	allocate_chunk(ptr_page_directory, (uint32) MemBlockNodes, MemBlock_size,
	PERM_WRITEABLE);

#endif
	//[3] Initialize AvailableMemBlocksList by filling it with the MemBlockNodes

	initialize_MemBlocksList(MAX_MEM_BLOCK_CNT);

	//[4] Insert a new MemBlock with the remaining heap size into the FreeMemBlocksList

	struct MemBlock *free = LIST_FIRST(&AvailableMemBlocksList);

	free->size = Kheap_size - MemBlock_size;
	free->sva = KERNEL_HEAP_START + MemBlock_size;

	LIST_REMOVE(&AvailableMemBlocksList, free);

	LIST_INSERT_HEAD(&FreeMemBlocksList, free);
}

void* kmalloc(unsigned int size) {
	//TODO: [PROJECT MS2] [KERNEL HEAP] kmalloc
	// your code is here, remove the panic and write your code
	//kpanic_into_prompt("kmalloc() is not implemented yet...!!");

	//NOTE: All kernel heap allocations are multiples of PAGE_SIZE (4KB)
	//refer to the project presentation and documentation for details
	// use "isKHeapPlacementStrategyFIRSTFIT() ..." functions to check the current strategy
	size = ROUNDUP(size, PAGE_SIZE);
	uint32 va = LIST_FIRST(&FreeMemBlocksList)->sva;
	if (KERNEL_HEAP_MAX - va < size || size == 0)
		return (void *) NULL;

	if (isKHeapPlacementStrategyFIRSTFIT() == 1) {
		struct MemBlock* block = alloc_block_FF(size);
		if (block != NULL) {
			int retrn = -1;
			va = block->sva;
			while (retrn == -1 && va != KERNEL_HEAP_MAX) {
				retrn = allocate_chunk(ptr_page_directory, va, size,
				PERM_PRESENT | PERM_WRITEABLE);
				va += PAGE_SIZE;
			}

			insert_sorted_allocList(block);

			return (void *) block->sva;
		}
	} else if (isKHeapPlacementStrategyBESTFIT() == 1) {
		struct MemBlock* block = alloc_block_BF(size);
		if (block != NULL) {
			int retrn = -1;
			va = block->sva;
			while (retrn == -1 && va <= KERNEL_HEAP_MAX) {
				retrn = allocate_chunk(ptr_page_directory, va, size,
				PERM_PRESENT | PERM_WRITEABLE);
				va += PAGE_SIZE;
			}

			insert_sorted_allocList(block);

			return (void *) block->sva;
		}
	} else {
		struct MemBlock* block = alloc_block_NF(size);
		if (block != NULL) {
			int retrn = -1;
			va = block->sva;
			while (retrn == -1 && va <= KERNEL_HEAP_MAX) {
				retrn = allocate_chunk(ptr_page_directory, va, size,
				PERM_PRESENT | PERM_WRITEABLE);
				va += PAGE_SIZE;
			}

			insert_sorted_allocList(block);

			return (void *) block->sva;
		}
	}
	return (void *) NULL;
	//change this "return" according to your answer
}

void kfree(void* virtual_address) {
	//TODO: [PROJECT MS2] [KERNEL HEAP] kfree
	// Write your code here, remove the panic and write your code
//	panic("kfree() is not implemented yet...!!");

	uint32 va = ROUNDDOWN((uint32 )virtual_address, PAGE_SIZE);

	struct MemBlock *block = find_block(&AllocMemBlocksList,
			(uint32) virtual_address);

	if (block != NULL)
	{
		for (int i = 0; i < block->size / PAGE_SIZE; i++) {
			uint32 physical_address = virtual_to_physical(ptr_page_directory,va);
			struct FrameInfo *ptr_frame_info;
			ptr_frame_info = to_frame_info(physical_address);
			free_frame(ptr_frame_info);
			unmap_frame(ptr_page_directory, va);

			va += PAGE_SIZE;
		}

		LIST_REMOVE(&AllocMemBlocksList, block);
		insert_sorted_with_merge_freeList(block);
	}

}

unsigned int kheap_virtual_address(unsigned int physical_address) {
	//TODO: [PROJECT MS2] [KERNEL HEAP] kheap_virtual_address
	// Write your code here, remove the panic and write your code
//	panic("kheap_virtual_address() is not implemented yet...!!");
//
//	struct FrameInfo *frame = to_frame_info(physical_address);
//
//	uint32 frame_num = to_frame_number(frame);
	return to_frame_info(physical_address)->va;

	//return the virtual address corresponding to given physical_address
	//refer to the project presentation and documentation for details
	//EFFICIENT IMPLEMENTATION ~O(1) IS REQUIRED ==================
}

unsigned int kheap_physical_address(unsigned int virtual_address) {
	//TODO: [PROJECT MS2] [KERNEL HEAP] kheap_physical_address
	// Write your code here, remove the panic and write your code
//	panic("kheap_physical_address() is not implemented yet...!!");

	return virtual_to_physical(ptr_page_directory, virtual_address);
	//return the physical address corresponding to given virtual_address
	//refer to the project presentation and documentation for details
}

void kfreeall() {
	panic("Not implemented!");

}

void kshrink(uint32 newSize) {
	panic("Not implemented!");
}

void kexpand(uint32 newSize) {
	panic("Not implemented!");
}

//=================================================================================//
//============================== BONUS FUNCTION ===================================//
//=================================================================================//
// krealloc():

//	Attempts to resize the allocated space at "virtual_address" to "new_size" bytes,
//	possibly moving it in the heap.
//	If successful, returns the new virtual_address, in which case the old virtual_address must no longer be accessed.
//	On failure, returns a null pointer, and the old virtual_address remains valid.

//	A call with virtual_address = null is equivalent to kmalloc().
//	A call with new_size = zero is equivalent to kfree().

void *krealloc(void *virtual_address, uint32 new_size) {
    //TODO: [PROJECT MS2 - BONUS] [KERNEL HEAP] krealloc
    // Write your code here, remove the panic and write your code
//    panic("krealloc() is not implemented yet...!!");



   if (virtual_address == NULL)
        return kmalloc(new_size);
    if (new_size == 0) {
        kfree(virtual_address);
        return (void *) NULL;
    }
    struct MemBlock *block = find_block(&AllocMemBlocksList,(uint32) virtual_address);
    int remainig_size = new_size - block->size;
    if (block != NULL)
    {
        if (remainig_size > 0) // remaining size +ve we can increase the block's size
        {
            struct MemBlock *next_block = find_block(&AllocMemBlocksList,(uint32) virtual_address + block->size);
            if (next_block != NULL)
            {
                uint32 dest = (uint32) kmalloc(ROUNDUP(new_size,PAGE_SIZE));
                copy_paste_chunk(ptr_page_directory,(uint32) virtual_address,dest,block->size);
                kfree(virtual_address);



               return (void *)dest;
            }
//             if it reached this line that means that the next block is free
         int ret = allocate_chunk(ptr_page_directory,(uint32)virtual_address + block->size, remainig_size, PERM_WRITEABLE);
         if(ret == -1)
         {
            uint32 dest = (uint32) kmalloc(new_size);
            copy_paste_chunk(ptr_page_directory,(uint32) virtual_address,dest,block->size);
            kfree(virtual_address);



           return (void *)dest;
         }
         block->size = ROUNDUP(new_size,PAGE_SIZE);
            return virtual_address;
        }
        else if (remainig_size < 0) // remaining size -ve
        {
            return (void *) block->sva;
        }
    }
    return (void *) NULL;
}
