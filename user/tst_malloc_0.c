/* *********************************************************** */
/* MAKE SURE PAGE_WS_MAX_SIZE = 1000 */
/* *********************************************************** */

#include <inc/lib.h>

void _main(void)
{
	//Initial test to ensure it works on "PLACEMENT" not "REPLACEMENT"
	{
		uint8 fullWS = 1;
		for (int i = 0; i < myEnv->page_WS_max_size; ++i)
		{
			if (myEnv->__uptr_pws[i].empty)
			{
				fullWS = 0;
				break;
			}
		}
		if (fullWS) panic("Please increase the WS size");
	}
	/*Dummy malloc to enforce the UHEAP initializations*/
	if(STATIC_MEMBLOCK_ALLOC != 0)
		panic("STATIC_MEMBLOCK_ALLOC = 1 & it shall be 0. Go to 'inc/dynamic_allocator.h' and set STATIC_MEMBLOCK_ALLOC by 0. Then, repeat the test again.");

	int freeFrames_before = sys_calculate_free_frames() ;
	int freeDiskFrames_before = sys_pf_calculate_allocated_pages() ;
	malloc(0);
	int freeFrames_after = sys_calculate_free_frames() ;
	int freeDiskFrames_after = sys_pf_calculate_allocated_pages() ;

	//Check MAX_MEM_BLOCK_CNT
	if(MAX_MEM_BLOCK_CNT != ((0xA0000000-0x80000000)/4096))
	{
		panic("Wrong initialize: MAX_MEM_BLOCK_CNT is not set with the correct size of the array");
	}

	//Check number of nodes in AvailableMemBlocksList
	if (LIST_SIZE(&(AvailableMemBlocksList)) != MAX_MEM_BLOCK_CNT-1)
	{
		panic("Wrong initialize: Wrong size for the AvailableMemBlocksList");
	}

	//Check number of nodes in AllocMemBlocksList
	if (LIST_SIZE(&(AllocMemBlocksList)) != 0)
	{
		panic("Wrong initialize: Wrong size for the AllocMemBlocksList");
	}

	//Check number of nodes in FreeMemBlocksList
	if (LIST_SIZE(&(FreeMemBlocksList)) != 1)
	{
		panic("Wrong initialize: Wrong size for the FreeMemBlocksList");
	}

	//Check content of FreeMemBlocksList
	struct MemBlock* block = LIST_FIRST(&FreeMemBlocksList);
	if(block == NULL || block->size != (0xA0000000-0x80000000) || block->sva != 0x80000000)
	{
		panic("Wrong initialize: Wrong content for the FreeMemBlocksList.");
	}

	//Check number of disk and memory frames
	if ((freeDiskFrames_after - freeDiskFrames_before) != 0) panic("Page file is changed while it's not expected to. (pages are wrongly allocated/de-allocated in PageFile)");
	if ((freeFrames_before - freeFrames_after) != 512 + 1) panic("Wrong allocation: pages are not loaded successfully into memory %d", (freeFrames_before - freeFrames_after));

	/*=================================================*/

	cprintf("Congratulations!! test initialize_dyn_block_system of UHEAP completed successfully.\n");

	return;
}
