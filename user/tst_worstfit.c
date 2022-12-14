/* *********************************************************** */
/* MAKE SURE PAGE_WS_MAX_SIZE = 1000 */
/* *********************************************************** */

#include <inc/lib.h>

void _main(void)
{
	sys_set_uheap_strategy(UHP_PLACE_WORSTFIT);

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
	malloc(0);
	/*=================================================*/

	int Mega = 1024*1024;
	int kilo = 1024;

	int count = 0;
	int totalNumberOfTests = 11;

	//Make sure that the heap size is 512 MB
	int numOf2MBsInHeap = (USER_HEAP_MAX - USER_HEAP_START) / (2*Mega);
	assert(numOf2MBsInHeap == 256);




	sys_set_uheap_strategy(UHP_PLACE_WORSTFIT);

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

	int freeFrames ;
	int usedDiskPages;

	cprintf("This test has %d tests. A pass message will be displayed after each one.\n", totalNumberOfTests);

	//[0] Make sure there're available places in the WS
	int w = 0 ;
	int requiredNumOfEmptyWSLocs = 2;
	int numOfEmptyWSLocs = 0;
	for (w = 0 ; w < myEnv->page_WS_max_size; w++)
	{
		if( myEnv->__uptr_pws[w].empty == 1)
			numOfEmptyWSLocs++;
	}
	if (numOfEmptyWSLocs < requiredNumOfEmptyWSLocs)
		panic("Insufficient number of WS empty locations! please increase the PAGE_WS_MAX_SIZE");

	void* ptr_allocations[512] = {0};
	int i;

	// allocate pages
	freeFrames = sys_calculate_free_frames() ;
	usedDiskPages = sys_pf_calculate_allocated_pages();
	for(i = 0; i< 256;i++)
	{
		ptr_allocations[i] = malloc(2*Mega);
	}

	// randomly check the addresses of the allocation
	if( (uint32)ptr_allocations[0] != 0x80000000 ||
			(uint32)ptr_allocations[2] != 0x80400000 ||
			(uint32)ptr_allocations[8] != 0x81000000 ||
			(uint32)ptr_allocations[100] != 0x8C800000 ||
			(uint32)ptr_allocations[150] != 0x92C00000 ||
			(uint32)ptr_allocations[200] != 0x99000000 ||
			(uint32)ptr_allocations[255] != 0x9FE00000)
		panic("Wrong allocation, Check fitting strategy is working correctly");

	if( (sys_pf_calculate_allocated_pages() - usedDiskPages) !=  512*Mega/PAGE_SIZE) panic("Wrong page file allocation: ");
	if ((freeFrames - sys_calculate_free_frames()) != (512*Mega)/(1024*PAGE_SIZE) ) panic("Wrong allocation");

	// Make memory holes.
	freeFrames = sys_calculate_free_frames() ;
	usedDiskPages = sys_pf_calculate_allocated_pages();

	free(ptr_allocations[0]);		// Hole 1 = 2 M
	free(ptr_allocations[2]);		// Hole 2 = 4 M
	free(ptr_allocations[3]);
	free(ptr_allocations[10]);		// Hole 3 = 6 M
	free(ptr_allocations[12]);
	free(ptr_allocations[11]);
	free(ptr_allocations[100]);		// Hole 4 = 10 M
	free(ptr_allocations[104]);
	free(ptr_allocations[103]);
	free(ptr_allocations[102]);
	free(ptr_allocations[101]);
	free(ptr_allocations[200]);		// Hole 5 = 8 M
	free(ptr_allocations[201]);
	free(ptr_allocations[202]);
	free(ptr_allocations[203]);

	if ((usedDiskPages - sys_pf_calculate_allocated_pages()) != 15*(2*Mega)/PAGE_SIZE) panic("Wrong free: Extra or less pages are removed from PageFile");
	if ((sys_calculate_free_frames() - freeFrames) != 0) panic("Wrong free: Extra or less pages are removed from main memory");

	// Test worst fit
	//[WORST FIT Case]
	freeFrames = sys_calculate_free_frames() ;
	usedDiskPages = sys_pf_calculate_allocated_pages();
	void* tempAddress = malloc(Mega);		// Use Hole 4 -> Hole 4 = 9 M
	if((uint32)tempAddress != 0x8C800000)
		panic("Worst Fit not working correctly");
	if( (sys_pf_calculate_allocated_pages() - usedDiskPages) !=  1*Mega/PAGE_SIZE) panic("Wrong page file allocation: ");
	if ((freeFrames - sys_calculate_free_frames()) != 0) panic("Wrong allocation");
	cprintf("Test %d Passed \n", ++count);

	freeFrames = sys_calculate_free_frames() ;
	usedDiskPages = sys_pf_calculate_allocated_pages();
	tempAddress = malloc(4 * Mega);			// Use Hole 4 -> Hole 4 = 5 M
	if((uint32)tempAddress != 0x8C900000)
		panic("Worst Fit not working correctly");
	if( (sys_pf_calculate_allocated_pages() - usedDiskPages) !=  4*Mega/PAGE_SIZE) panic("Wrong page file allocation: ");
	if ((freeFrames - sys_calculate_free_frames()) != 0) panic("Wrong allocation");
	cprintf("Test %d Passed \n", ++count);

	freeFrames = sys_calculate_free_frames() ;
	usedDiskPages = sys_pf_calculate_allocated_pages();
	tempAddress = malloc(6*Mega); 			   // Use Hole 5 -> Hole 5 = 2 M
	if((uint32)tempAddress != 0x99000000)
		panic("Worst Fit not working correctly");
	if( (sys_pf_calculate_allocated_pages() - usedDiskPages) !=  6*Mega/PAGE_SIZE) panic("Wrong page file allocation: ");
	if ((freeFrames - sys_calculate_free_frames()) != 0) panic("Wrong allocation");
	cprintf("Test %d Passed \n", ++count);

	freeFrames = sys_calculate_free_frames() ;
	usedDiskPages = sys_pf_calculate_allocated_pages();
	tempAddress = malloc(5*Mega); 			   // Use Hole 3 -> Hole 3 = 1 M
	if((uint32)tempAddress != 0x81400000)
		panic("Worst Fit not working correctly");
	if( (sys_pf_calculate_allocated_pages() - usedDiskPages) !=  5*Mega/PAGE_SIZE) panic("Wrong page file allocation: ");
	if ((freeFrames - sys_calculate_free_frames()) != 0) panic("Wrong allocation");
	cprintf("Test %d Passed \n", ++count);

	freeFrames = sys_calculate_free_frames() ;
	usedDiskPages = sys_pf_calculate_allocated_pages();
	tempAddress = malloc(4*Mega); 			   // Use Hole 4 -> Hole 4 = 1 M
	if((uint32)tempAddress != 0x8CD00000)
		panic("Worst Fit not working correctly");
	if( (sys_pf_calculate_allocated_pages() - usedDiskPages) !=  4*Mega/PAGE_SIZE) panic("Wrong page file allocation: ");
	if ((freeFrames - sys_calculate_free_frames()) != 0) panic("Wrong allocation");
	cprintf("Test %d Passed \n", ++count);

	freeFrames = sys_calculate_free_frames() ;
	usedDiskPages = sys_pf_calculate_allocated_pages();
	tempAddress = malloc(2 * Mega); 			// Use Hole 2 -> Hole 2 = 2 M
	if((uint32)tempAddress != 0x80400000)
		panic("Worst Fit not working correctly");
	if( (sys_pf_calculate_allocated_pages() - usedDiskPages) !=  2*Mega/PAGE_SIZE) panic("Wrong page file allocation: ");
	if ((freeFrames - sys_calculate_free_frames()) != 0) panic("Wrong allocation");
	cprintf("Test %d Passed \n", ++count);

	freeFrames = sys_calculate_free_frames() ;
	usedDiskPages = sys_pf_calculate_allocated_pages();
	tempAddress = malloc(1*Mega + 512*kilo);    // Use Hole 1 -> Hole 1 = 0.5 M
	if((uint32)tempAddress != 0x80000000)
		panic("Worst Fit not working correctly");
	if( (sys_pf_calculate_allocated_pages() - usedDiskPages) !=  (1*Mega + 512*kilo)/PAGE_SIZE) panic("Wrong page file allocation: ");
	if ((freeFrames - sys_calculate_free_frames()) != 0) panic("Wrong allocation");
	cprintf("Test %d Passed \n", ++count);

	freeFrames = sys_calculate_free_frames() ;
	usedDiskPages = sys_pf_calculate_allocated_pages();
	tempAddress = malloc(512*kilo); 			   // Use Hole 2 -> Hole 2 = 1.5 M
	if((uint32)tempAddress != 0x80600000)
		panic("Worst Fit not working correctly");
	if( (sys_pf_calculate_allocated_pages() - usedDiskPages) !=  512*kilo/PAGE_SIZE) panic("Wrong page file allocation: ");
	if ((freeFrames - sys_calculate_free_frames()) != 0) panic("Wrong allocation");
	cprintf("Test %d Passed \n", ++count);

	freeFrames = sys_calculate_free_frames() ;
	usedDiskPages = sys_pf_calculate_allocated_pages();
	tempAddress = malloc(kilo); 			   // Use Hole 5 -> Hole 5 = 2 M - K
	if((uint32)tempAddress != 0x99600000)
		panic("Worst Fit not working correctly");
	if( (sys_pf_calculate_allocated_pages() - usedDiskPages) !=  4*kilo/PAGE_SIZE) panic("Wrong page file allocation: ");
	if ((freeFrames - sys_calculate_free_frames()) != 0) panic("Wrong allocation");
	cprintf("Test %d Passed \n", ++count);

	freeFrames = sys_calculate_free_frames() ;
	usedDiskPages = sys_pf_calculate_allocated_pages();
	tempAddress = malloc(2*Mega - 4*kilo); 		// Use Hole 5 -> Hole 5 = 0
	if((uint32)tempAddress != 0x99601000)
		panic("Worst Fit not working correctly");
	if( (sys_pf_calculate_allocated_pages() - usedDiskPages) !=  (2*Mega - 4*kilo)/PAGE_SIZE) panic("Wrong page file allocation: ");
	if ((freeFrames - sys_calculate_free_frames()) != 0) panic("Wrong allocation");
	cprintf("Test %d Passed \n", ++count);

	// Check that worst fit returns null in case all holes are not free
	freeFrames = sys_calculate_free_frames() ;
	usedDiskPages = sys_pf_calculate_allocated_pages();
	tempAddress = malloc(4*Mega); 		//No Suitable hole
	if((uint32)tempAddress != 0x0)
		panic("Worst Fit not working correctly");
	if( (sys_pf_calculate_allocated_pages() - usedDiskPages) !=  0) panic("Wrong page file allocation: ");
	if ((freeFrames - sys_calculate_free_frames()) != 0) panic("Wrong allocation");
	cprintf("Test %d Passed \n", ++count);

	cprintf("Congratulations!! test Worst Fit completed successfully.\n");

	return;
}
