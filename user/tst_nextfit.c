/* *********************************************************** */
/* MAKE SURE PAGE_WS_MAX_SIZE = 2000 */
/* *********************************************************** */

#include <inc/lib.h>

void _main(void)
{
	int Mega = 1024*1024;
	int kilo = 1024;
	sys_set_uheap_strategy(UHP_PLACE_NEXTFIT);

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

	//Make sure that the heap size is 512 MB
	int numOf2MBsInHeap = (USER_HEAP_MAX - USER_HEAP_START) / (2*Mega);
	assert(numOf2MBsInHeap == 256);




	sys_set_uheap_strategy(UHP_PLACE_NEXTFIT);

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

	//[0] Make sure there're available places in the WS
	int w = 0 ;
	int requiredNumOfEmptyWSLocs = 2;
	int numOfEmptyWSLocs = 0;
	for (w = 0 ; w < myEnv->page_WS_max_size ; w++)
	{
		if( myEnv->__uptr_pws[w].empty == 1)
			numOfEmptyWSLocs++;
	}
	if (numOfEmptyWSLocs < requiredNumOfEmptyWSLocs)
		panic("Insufficient number of WS empty locations! please increase the PAGE_WS_MAX_SIZE");


	void* ptr_allocations[512] = {0};
	int i;

	cprintf("This test has THREE cases. A pass message will be displayed after each one.\n");

	// allocate pages
	freeFrames = sys_calculate_free_frames() ;
	usedDiskPages = sys_pf_calculate_allocated_pages();
	for(i = 0; i< 256;i++)
	{
		ptr_allocations[i] = malloc(2*Mega);
	}

	// randomly check the addresses of the allocation
	if( 	(uint32)ptr_allocations[0] != 0x80000000 ||
			(uint32)ptr_allocations[2] != 0x80400000 ||
			(uint32)ptr_allocations[8] != 0x81000000 ||
			(uint32)ptr_allocations[10] != 0x81400000 ||
			(uint32)ptr_allocations[15] != 0x81e00000 ||
			(uint32)ptr_allocations[20] != 0x82800000 ||
			(uint32)ptr_allocations[25] != 0x83200000 ||
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
	free(ptr_allocations[5]);		// Hole 3 = 2 M
	free(ptr_allocations[10]);		// Hole 4 = 6 M
	free(ptr_allocations[12]);
	free(ptr_allocations[11]);
	free(ptr_allocations[20]);		// Hole 5 = 2 M
	free(ptr_allocations[25]);		// Hole 6 = 2 M
	free(ptr_allocations[255]);		// Hole 7 = 2 M

	if ((usedDiskPages - sys_pf_calculate_allocated_pages()) != 10*(2*Mega)/PAGE_SIZE) panic("Wrong free: Extra or less pages are removed from PageFile");
	if ((sys_calculate_free_frames() - freeFrames) != 0) panic("Wrong free: Extra or less pages are removed from main memory");

	// Test next fit

	freeFrames = sys_calculate_free_frames() ;
	usedDiskPages = sys_pf_calculate_allocated_pages();
	void* tempAddress = malloc(Mega-kilo);		// Use Hole 1 -> Hole 1 = 1 M
	if((uint32)tempAddress != 0x80000000)
		panic("Next Fit not working correctly");
	if( (sys_pf_calculate_allocated_pages() - usedDiskPages) !=  1*Mega/PAGE_SIZE) panic("Wrong page file allocation: ");
	if ((freeFrames - sys_calculate_free_frames()) != 0) panic("Wrong allocation");

	freeFrames = sys_calculate_free_frames() ;
	usedDiskPages = sys_pf_calculate_allocated_pages();
	tempAddress = malloc(kilo);					// Use Hole 1 -> Hole 1 = 1 M - Kilo
	if((uint32)tempAddress != 0x80100000)
		panic("Next Fit not working correctly");
	if( (sys_pf_calculate_allocated_pages() - usedDiskPages) !=  4*kilo/PAGE_SIZE) panic("Wrong page file allocation: ");
	if ((freeFrames - sys_calculate_free_frames()) != 0) panic("Wrong allocation");

	freeFrames = sys_calculate_free_frames() ;
	usedDiskPages = sys_pf_calculate_allocated_pages();
	tempAddress = malloc(5*Mega); 			   // Use Hole 4 -> Hole 4 = 1 M
	if((uint32)tempAddress != 0x81400000)
		panic("Next Fit not working correctly");
	if( (sys_pf_calculate_allocated_pages() - usedDiskPages) !=  5*Mega/PAGE_SIZE) panic("Wrong page file allocation: ");
	if ((freeFrames - sys_calculate_free_frames()) != 0) panic("Wrong allocation");

	freeFrames = sys_calculate_free_frames() ;
	usedDiskPages = sys_pf_calculate_allocated_pages();
	tempAddress = malloc(1*Mega); 			   // Use Hole 4 -> Hole 4 = 0 M
	if((uint32)tempAddress != 0x81900000)
		panic("Next Fit not working correctly");
	if( (sys_pf_calculate_allocated_pages() - usedDiskPages) !=  1*Mega/PAGE_SIZE) panic("Wrong page file allocation: ");
	if ((freeFrames - sys_calculate_free_frames()) != 0) panic("Wrong allocation");


	freeFrames = sys_calculate_free_frames() ;
	usedDiskPages = sys_pf_calculate_allocated_pages();
	free(ptr_allocations[15]);					// Make a new hole => 2 M
	if ((usedDiskPages - sys_pf_calculate_allocated_pages()) != 2*Mega/PAGE_SIZE) panic("Wrong free: Extra or less pages are removed from PageFile");
	if ((sys_calculate_free_frames() - freeFrames) != 0) panic("Wrong free: Extra or less pages are removed from main memory");

	//[NEXT FIT Case]
	freeFrames = sys_calculate_free_frames() ;
	usedDiskPages = sys_pf_calculate_allocated_pages();
	tempAddress = malloc(kilo); 			   // Use new Hole = 2 M - 4 kilo
	if((uint32)tempAddress != 0x81E00000)
		panic("Next Fit not working correctly");
	if( (sys_pf_calculate_allocated_pages() - usedDiskPages) !=  4*kilo/PAGE_SIZE) panic("Wrong page file allocation: ");
	if ((freeFrames - sys_calculate_free_frames()) != 0) panic("Wrong allocation");

	freeFrames = sys_calculate_free_frames() ;
	usedDiskPages = sys_pf_calculate_allocated_pages();
	tempAddress = malloc(Mega + 1016*kilo); 	// Use new Hole = 4 kilo
	if((uint32)tempAddress != 0x81E01000)
		panic("Next Fit not working correctly");
	if( (sys_pf_calculate_allocated_pages() - usedDiskPages) !=  (1*Mega+1016*kilo)/PAGE_SIZE) panic("Wrong page file allocation: ");
	if ((freeFrames - sys_calculate_free_frames()) != 0) panic("Wrong allocation");

	freeFrames = sys_calculate_free_frames() ;
	usedDiskPages = sys_pf_calculate_allocated_pages();
	tempAddress = malloc(512*kilo); 			   // Use Hole 5 -> Hole 5 = 1.5 M
	if((uint32)tempAddress != 0x82800000)
		panic("Next Fit not working correctly");
	if( (sys_pf_calculate_allocated_pages() - usedDiskPages) !=  512*kilo/PAGE_SIZE) panic("Wrong page file allocation: ");
	if ((freeFrames - sys_calculate_free_frames()) != 0) panic("Wrong allocation");

	cprintf("\nCASE1: (next fit without looping back) is succeeded...\n") ;

	// Check that next fit is looping back to check for free space
	freeFrames = sys_calculate_free_frames() ;
	usedDiskPages = sys_pf_calculate_allocated_pages();
	tempAddress = malloc(3*Mega + 512*kilo); 			   // Use Hole 2 -> Hole 2 = 0.5 M
	if((uint32)tempAddress != 0x80400000)
		panic("Next Fit not working correctly");
	if( (sys_pf_calculate_allocated_pages() - usedDiskPages) !=  (3*Mega+512*kilo)/PAGE_SIZE) panic("Wrong page file allocation: ");
	if ((freeFrames - sys_calculate_free_frames()) != 0) panic("Wrong allocation");


	freeFrames = sys_calculate_free_frames() ;
	usedDiskPages = sys_pf_calculate_allocated_pages();
	free(ptr_allocations[24]);		// Increase size of Hole 6 to 4 M
	if ((usedDiskPages - sys_pf_calculate_allocated_pages()) != 2*Mega/PAGE_SIZE) panic("Wrong free: Extra or less pages are removed from PageFile");
	if ((sys_calculate_free_frames() - freeFrames) != 0) panic("Wrong free: Extra or less pages are removed from main memory");


	freeFrames = sys_calculate_free_frames() ;
	usedDiskPages = sys_pf_calculate_allocated_pages();
	tempAddress = malloc(4*Mega-kilo);		// Use Hole 6 -> Hole 6 = 0 M
	if((uint32)tempAddress != 0x83000000)
		panic("Next Fit not working correctly");
	if( (sys_pf_calculate_allocated_pages() - usedDiskPages) !=  4*Mega/PAGE_SIZE) panic("Wrong page file allocation: ");
	if ((freeFrames - sys_calculate_free_frames()) != 0) panic("Wrong allocation");

	cprintf("\nCASE2: (next fit WITH looping back) is succeeded...\n") ;

	// Check that next fit returns null in case all holes are not free
	freeFrames = sys_calculate_free_frames() ;
	usedDiskPages = sys_pf_calculate_allocated_pages();
	tempAddress = malloc(6*Mega); 			   // No Suitable Hole is available
	if((uint32)tempAddress != 0x0)
		panic("Next Fit not working correctly");
	if( (sys_pf_calculate_allocated_pages() - usedDiskPages) !=  0) panic("Wrong page file allocation: ");
	if ((freeFrames - sys_calculate_free_frames()) != 0) panic("Wrong allocation");

	cprintf("\nCASE3: (next fit with insufficient space) is succeeded...\n") ;

	cprintf("Congratulations!! test Next Fit completed successfully.\n");

	return;
}
