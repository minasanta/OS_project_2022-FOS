#include <inc/lib.h>
extern void sys_clear_ffl() ;
int numOfExistPages ;
char* ptr = (char* )0x0801000 ;
char* ptr2 = (char* )0x080400A ;
char* ptr3 = (char* )(USTACKTOP - PTSIZE) ;
char* ptr4 = (char* )(USTACKTOP - 2*PTSIZE - 5*PAGE_SIZE) ;


char arr[PAGE_SIZE*12];
uint32 WSEntries_before[1000];

void _main(void)
{
	vcprintf("\n\n===============================================================\n", NULL);
	vcprintf("MAKE SURE to have a FRESH RUN for EACH SCENARIO of this test\n(i.e. don't run any program/test/multiple scenarios before it)\n", NULL);
	vcprintf("===============================================================\n\n\n", NULL);

	uint32 testCase;
	if (myEnv->page_WS_max_size == 1000)
	{
		//EVALUATION [40%]
		testCase = 1 ;
	}
	else if (myEnv->page_WS_max_size == 10)
	{
		//EVALUATION [30%]
		testCase = 2 ;
	}
	else if (myEnv->page_WS_max_size == 26)
	{
		//EVALUATION [30%]
		testCase = 3 ;
	}
	int32 envIdFib, envIdHelloWorld, helloWorldFrames;
	{
		//CASE1: free the exited env only
		//CASE3: free BOTH exited env's and WS
		if (testCase == 1 || testCase == 3)
		{
			//Load "fib" & "fos_helloWorld" programs into RAM
			cprintf("Loading Fib & fos_helloWorld programs into RAM...");
			envIdFib = sys_create_env("fib", (myEnv->page_WS_max_size),(myEnv->SecondListSize), (myEnv->percentage_of_WS_pages_to_be_removed));
			int freeFrames = sys_calculate_free_frames() ;
			envIdHelloWorld = sys_create_env("fos_helloWorld", (myEnv->page_WS_max_size), (myEnv->SecondListSize),(myEnv->percentage_of_WS_pages_to_be_removed));
			helloWorldFrames = freeFrames - sys_calculate_free_frames() ;
			env_sleep(2000);
			vcprintf("[DONE]\n\n", NULL);

			//Load and run "fos_add"
			cprintf("Loading fos_add program into RAM...");
			int32 envIdFOSAdd= sys_create_env("fos_add", (myEnv->page_WS_max_size), (myEnv->SecondListSize),(myEnv->percentage_of_WS_pages_to_be_removed));
			env_sleep(2000);
			vcprintf("[DONE]\n\n", NULL);

			cprintf("running fos_add program...\n\n");
			sys_run_env(envIdFOSAdd);

			cprintf("please be patient ...\n");
			env_sleep(5000);

		}
		/*		//CASE2: free the WS ONLY using clock algorithm
		else if (testCase == 2)
		{
			//("STEP 0: checking InitialWSError2: INITIAL WS entries ...\n");
			{
				if( ROUNDDOWN(myEnv->__uptr_pws[0].virtual_address,PAGE_SIZE) !=   0x200000)  	panic("InitialWSError2: INITIAL PAGE WS entry checking failed! Review size of the WS..!!");
				if( ROUNDDOWN(myEnv->__uptr_pws[1].virtual_address,PAGE_SIZE) !=   0x201000)  panic("InitialWSError2: INITIAL PAGE WS entry checking failed! Review size of the WS..!!");
				if( ROUNDDOWN(myEnv->__uptr_pws[2].virtual_address,PAGE_SIZE) !=   0x202000)  panic("InitialWSError2: INITIAL PAGE WS entry checking failed! Review size of the WS..!!");
				if( ROUNDDOWN(myEnv->__uptr_pws[3].virtual_address,PAGE_SIZE) !=   0x203000)  panic("InitialWSError2: INITIAL PAGE WS entry checking failed! Review size of the WS..!!");
				if( ROUNDDOWN(myEnv->__uptr_pws[4].virtual_address,PAGE_SIZE) !=   0x204000)  panic("InitialWSError2: INITIAL PAGE WS entry checking failed! Review size of the WS..!!");
				if( ROUNDDOWN(myEnv->__uptr_pws[5].virtual_address,PAGE_SIZE) !=   0x205000)  panic("InitialWSError2: INITIAL PAGE WS entry checking failed! Review size of the WS..!!");
				if( ROUNDDOWN(myEnv->__uptr_pws[6].virtual_address,PAGE_SIZE) !=   0x800000)  panic("InitialWSError2: INITIAL PAGE WS entry checking failed! Review size of the WS..!!");
				if( ROUNDDOWN(myEnv->__uptr_pws[7].virtual_address,PAGE_SIZE) !=   0x801000)  panic("InitialWSError2: INITIAL PAGE WS entry checking failed! Review size of the WS..!!");
				if( ROUNDDOWN(myEnv->__uptr_pws[8].virtual_address,PAGE_SIZE) !=   0x802000)  panic("InitialWSError2: INITIAL PAGE WS entry checking failed! Review size of the WS..!!");
				if( ROUNDDOWN(myEnv->__uptr_pws[9].virtual_address,PAGE_SIZE) !=   0xeebfd000)  panic("InitialWSError2: INITIAL PAGE WS entry checking failed! Review size of the WS..!!");
				if( myEnv->page_last_WS_index !=  0)  										panic("InitialWSError2: INITIAL PAGE WS last index checking failed! Review size of the WS..!!");
			}
		}
		 */
		//CASE2: free the WS ONLY using FIFO algorithm
		else if (testCase == 2)
		{
			//("STEP 0: checking InitialWSError2: INITIAL WS entries ...\n");
			{
				if( ROUNDDOWN(myEnv->__uptr_pws[0].virtual_address,PAGE_SIZE) !=   0x804000)  	panic("InitialWSError2: INITIAL PAGE WS entry checking failed! Review size of the WS..!!");
				if( ROUNDDOWN(myEnv->__uptr_pws[1].virtual_address,PAGE_SIZE) !=   0x201000)  panic("InitialWSError2: INITIAL PAGE WS entry checking failed! Review size of the WS..!!");
				if( ROUNDDOWN(myEnv->__uptr_pws[2].virtual_address,PAGE_SIZE) !=   0x202000)  panic("InitialWSError2: INITIAL PAGE WS entry checking failed! Review size of the WS..!!");
				if( ROUNDDOWN(myEnv->__uptr_pws[3].virtual_address,PAGE_SIZE) !=   0x203000)  panic("InitialWSError2: INITIAL PAGE WS entry checking failed! Review size of the WS..!!");
				if( ROUNDDOWN(myEnv->__uptr_pws[4].virtual_address,PAGE_SIZE) !=   0x204000)  panic("InitialWSError2: INITIAL PAGE WS entry checking failed! Review size of the WS..!!");
				if( ROUNDDOWN(myEnv->__uptr_pws[5].virtual_address,PAGE_SIZE) !=   0x205000)  panic("InitialWSError2: INITIAL PAGE WS entry checking failed! Review size of the WS..!!");
				if( ROUNDDOWN(myEnv->__uptr_pws[6].virtual_address,PAGE_SIZE) !=   0x800000)  panic("InitialWSError2: INITIAL PAGE WS entry checking failed! Review size of the WS..!!");
				if( ROUNDDOWN(myEnv->__uptr_pws[7].virtual_address,PAGE_SIZE) !=   0x801000)  panic("InitialWSError2: INITIAL PAGE WS entry checking failed! Review size of the WS..!!");
				if( ROUNDDOWN(myEnv->__uptr_pws[8].virtual_address,PAGE_SIZE) !=   0x802000)  panic("InitialWSError2: INITIAL PAGE WS entry checking failed! Review size of the WS..!!");
				if( ROUNDDOWN(myEnv->__uptr_pws[9].virtual_address,PAGE_SIZE) !=   0xeebfd000)  panic("InitialWSError2: INITIAL PAGE WS entry checking failed! Review size of the WS..!!");
				if( myEnv->page_last_WS_index !=  1)  										panic("InitialWSError2: INITIAL PAGE WS last index checking failed! Review size of the WS..!!");
			}
		}

		//Reading (Not Modified)
		char garbage1 = arr[PAGE_SIZE*10-1] ;
		char garbage2 = arr[PAGE_SIZE*11-1] ;
		char garbage3 = arr[PAGE_SIZE*12-1] ;

		char garbage4, garbage5 ;
		//Writing (Modified)
		int i ;
		for (i = 0 ; i < PAGE_SIZE*4 ; i+=PAGE_SIZE/2)
		{
			arr[i] = -1 ;
			//always use pages at 0x801000 and 0x804000
			garbage4 = *ptr ;
			garbage5 = *ptr2 ;
		}

		//===================

		//CASE1: free the exited env only
		//CASE3: free BOTH exited env's and WS
		if (testCase == 1 || testCase == 3)
		{
			int i = 0;
			numOfExistPages = 0;
			for (i = 0; i < myEnv->page_WS_max_size; ++i)
			{
				if (!myEnv->__uptr_pws[i].empty)
				{
					WSEntries_before[numOfExistPages++] = ROUNDDOWN(myEnv->__uptr_pws[i].virtual_address, PAGE_SIZE);
				}
			}
		}
		/*		//CASE2: free the WS ONLY using clock algorithm
		else if (testCase == 2)
		{
			//cprintf("Checking PAGE CLOCK algorithm... \n");
			{
				if( ROUNDDOWN(myEnv->__uptr_pws[0].virtual_address,PAGE_SIZE) !=  0x804000)  panic("Page CLOCK algo failed.. trace it by printing WS before and after page fault");
				if( ROUNDDOWN(myEnv->__uptr_pws[1].virtual_address,PAGE_SIZE) !=  0x80f000)  panic("Page CLOCK algo failed.. trace it by printing WS before and after page fault");
				if( ROUNDDOWN(myEnv->__uptr_pws[2].virtual_address,PAGE_SIZE) !=  0x810000)  panic("Page CLOCK algo failed.. trace it by printing WS before and after page fault");
				if( ROUNDDOWN(myEnv->__uptr_pws[3].virtual_address,PAGE_SIZE) !=  0x804000)  panic("Page CLOCK algo failed.. trace it by printing WS before and after page fault");
				if( ROUNDDOWN(myEnv->__uptr_pws[4].virtual_address,PAGE_SIZE) !=  0x805000)  panic("Page CLOCK algo failed.. trace it by printing WS before and after page fault");
				if( ROUNDDOWN(myEnv->__uptr_pws[5].virtual_address,PAGE_SIZE) !=  0x806000)  panic("Page CLOCK algo failed.. trace it by printing WS before and after page fault");
				if( ROUNDDOWN(myEnv->__uptr_pws[6].virtual_address,PAGE_SIZE) !=  0x800000)  panic("Page CLOCK algo failed.. trace it by printing WS before and after page fault");
				if( ROUNDDOWN(myEnv->__uptr_pws[7].virtual_address,PAGE_SIZE) !=  0x801000)  panic("Page CLOCK algo failed.. trace it by printing WS before and after page fault");
				if( ROUNDDOWN(myEnv->__uptr_pws[8].virtual_address,PAGE_SIZE) !=  0x807000)  panic("Page CLOCK algo failed.. trace it by printing WS before and after page fault");
				if( ROUNDDOWN(myEnv->__uptr_pws[9].virtual_address,PAGE_SIZE) !=  0xeebfd000)  panic("Page CLOCK algo failed.. trace it by printing WS before and after page fault");

				if(myEnv->page_last_WS_index != 9) panic("wrong PAGE WS pointer location");
			}
		}
		 */
		//CASE2: free the WS ONLY using FIFO algorithm
		else if (testCase == 2)
		{
			//cprintf("Checking PAGE FIFO algorithm... \n");
			{
				if( ROUNDDOWN(myEnv->__uptr_pws[0].virtual_address,PAGE_SIZE) !=  0x804000)  panic("Page FIFO algo failed.. trace it by printing WS before and after page fault");
				if( ROUNDDOWN(myEnv->__uptr_pws[1].virtual_address,PAGE_SIZE) !=  0x80e000)  panic("Page FIFO algo failed.. trace it by printing WS before and after page fault");
				if( ROUNDDOWN(myEnv->__uptr_pws[2].virtual_address,PAGE_SIZE) !=  0x80f000)  panic("Page FIFO algo failed.. trace it by printing WS before and after page fault");
				if( ROUNDDOWN(myEnv->__uptr_pws[3].virtual_address,PAGE_SIZE) !=  0x810000)  panic("Page FIFO algo failed.. trace it by printing WS before and after page fault");
				if( ROUNDDOWN(myEnv->__uptr_pws[4].virtual_address,PAGE_SIZE) !=  0x805000)  panic("Page FIFO algo failed.. trace it by printing WS before and after page fault");
				if( ROUNDDOWN(myEnv->__uptr_pws[5].virtual_address,PAGE_SIZE) !=  0x806000)  panic("Page FIFO algo failed.. trace it by printing WS before and after page fault");
				if( ROUNDDOWN(myEnv->__uptr_pws[6].virtual_address,PAGE_SIZE) !=  0x807000)  panic("Page FIFO algo failed.. trace it by printing WS before and after page fault");
				if( ROUNDDOWN(myEnv->__uptr_pws[7].virtual_address,PAGE_SIZE) !=  0x800000)  panic("Page FIFO algo failed.. trace it by printing WS before and after page fault");
				if( ROUNDDOWN(myEnv->__uptr_pws[8].virtual_address,PAGE_SIZE) !=  0x801000)  panic("Page FIFO algo failed.. trace it by printing WS before and after page fault");
				if( ROUNDDOWN(myEnv->__uptr_pws[9].virtual_address,PAGE_SIZE) !=  0xeebfd000)  panic("Page FIFO algo failed.. trace it by printing WS before and after page fault");

				if(myEnv->page_last_WS_index != 9) panic("wrong PAGE WS pointer location");
			}
		}

		//=========================================================//
		//Clear the FFL
		sys_clear_ffl();
		//=========================================================//

		//Writing (Modified) after freeing the entire FFL:
		//	3 frames should be allocated (stack page, mem table, page file table)
		*ptr3 = garbage1 ;
		//always use pages at 0x801000 and 0x804000
		garbage4 = *ptr ;
		garbage5 = *ptr2 ;

		//CASE1: free the exited env's ONLY
		//CASE3: free BOTH exited env's and WS
		if (testCase == 1 || testCase == 3)
		{
			//Add the last reference to our WS
			WSEntries_before[numOfExistPages++] = ROUNDDOWN((uint32)(ptr3), PAGE_SIZE);

			//Make sure that WS is not affected
			for (i = 0; i < numOfExistPages; ++i)
			{
				if (WSEntries_before[i] != ROUNDDOWN(myEnv->__uptr_pws[i].virtual_address, PAGE_SIZE))
					panic("FreeRAM.Scenario1 or 3: WS is changed while not expected to!");
			}
		}
		//Case2: free the WS ONLY by clock algorithm
		else if (testCase == 2)
		{
			/*//Check the WS after clock algorithm
			int i;
			for (i = 0 ; i < myEnv->page_WS_max_size; i++)
			{
				if (ROUNDDOWN(myEnv->__uptr_pws[i].virtual_address,PAGE_SIZE) ==  0x805000 ||
						ROUNDDOWN(myEnv->__uptr_pws[i].virtual_address,PAGE_SIZE) ==  0x80F000 ||
						ROUNDDOWN(myEnv->__uptr_pws[i].virtual_address,PAGE_SIZE) ==  0x810000)
					panic("test failed! either wrong victim or victim is not removed from WS");
			}
			 */

			//Check the WS after FIFO algorithm

			if (garbage4 != *ptr || garbage5 != *ptr2) panic("test failed!");

			//There should be two empty locations that are freed for the two tables (mem table and page file table)
			int numOfEmptyLocs = 0 ;
			for (int i = 0 ; i < myEnv->page_WS_max_size; i++)
			{
				if (myEnv->__uptr_pws[i].empty)
					numOfEmptyLocs++ ;
			}
			if (numOfEmptyLocs != 2) panic("test failed! either wrong victim or victim is not removed from WS");

			uint32 expectedAddresses[8] = {0x800000,0x801000,0x802000,0x803000,0x804000,0x807000,0xee7fe000,0xeebfd000} ;
			int numOfFoundedAddresses = 0;
			for (int j = 0; j < 8; j++)
			{
				for (int i = 0 ; i < myEnv->page_WS_max_size; i++)
				{
					if (ROUNDDOWN(myEnv->__uptr_pws[i].virtual_address,PAGE_SIZE) == expectedAddresses[j])
					{
						numOfFoundedAddresses++;
						break;
					}
				}
			}
			if (numOfFoundedAddresses != 8) panic("test failed! either wrong victim or victim is not removed from WS");

		}


		//Case1: free the exited env's ONLY
		if (testCase ==1)
		{
			cprintf("running fos_helloWorld program...\n\n");
			sys_run_env(envIdHelloWorld);
			cprintf("please be patient ...\n");
			env_sleep(3000);

			cprintf("running fos_fib program...\n\n");
			sys_run_env(envIdFib);
			cprintf("please be patient ...\n");
			env_sleep(5000);
		}
		//CASE3: free BOTH exited env's and WS
		else if (testCase ==3)
		{
			/*//cprintf("Checking PAGE CLOCK algorithm... \n");
			{
				if( ROUNDDOWN(myEnv->__uptr_pws[0].virtual_address,PAGE_SIZE) !=   0x200000)  	panic("InitialWSError2: INITIAL PAGE WS entry checking failed! Review size of the WS..!!");
				if( ROUNDDOWN(myEnv->__uptr_pws[1].virtual_address,PAGE_SIZE) !=   0x201000)  panic("InitialWSError2: INITIAL PAGE WS entry checking failed! Review size of the WS..!!");
				if( ROUNDDOWN(myEnv->__uptr_pws[2].virtual_address,PAGE_SIZE) !=   0x202000)  panic("InitialWSError2: INITIAL PAGE WS entry checking failed! Review size of the WS..!!");
				if( ROUNDDOWN(myEnv->__uptr_pws[3].virtual_address,PAGE_SIZE) !=   0x203000)  panic("InitialWSError2: INITIAL PAGE WS entry checking failed! Review size of the WS..!!");
				if( ROUNDDOWN(myEnv->__uptr_pws[4].virtual_address,PAGE_SIZE) !=   0x204000)  panic("InitialWSError2: INITIAL PAGE WS entry checking failed! Review size of the WS..!!");
				if( ROUNDDOWN(myEnv->__uptr_pws[5].virtual_address,PAGE_SIZE) !=   0x205000)  panic("InitialWSError2: INITIAL PAGE WS entry checking failed! Review size of the WS..!!");
				if( ROUNDDOWN(myEnv->__uptr_pws[6].virtual_address,PAGE_SIZE) !=   0x800000)  panic("InitialWSError2: INITIAL PAGE WS entry checking failed! Review size of the WS..!!");
				if( ROUNDDOWN(myEnv->__uptr_pws[7].virtual_address,PAGE_SIZE) !=   0x801000)  panic("InitialWSError2: INITIAL PAGE WS entry checking failed! Review size of the WS..!!");
				if( ROUNDDOWN(myEnv->__uptr_pws[8].virtual_address,PAGE_SIZE) !=   0x802000)  panic("InitialWSError2: INITIAL PAGE WS entry checking failed! Review size of the WS..!!");
				if( ROUNDDOWN(myEnv->__uptr_pws[9].virtual_address,PAGE_SIZE) !=   0x803000)  panic("InitialWSError2: INITIAL PAGE WS entry checking failed! Review size of the WS..!!");
				if( ROUNDDOWN(myEnv->__uptr_pws[10].virtual_address,PAGE_SIZE) !=   0x804000)  panic("InitialWSError2: INITIAL PAGE WS entry checking failed! Review size of the WS..!!");
				if( ROUNDDOWN(myEnv->__uptr_pws[11].virtual_address,PAGE_SIZE) !=   0x805000)  panic("InitialWSError2: INITIAL PAGE WS entry checking failed! Review size of the WS..!!");
				if( ROUNDDOWN(myEnv->__uptr_pws[12].virtual_address,PAGE_SIZE) !=   0x806000)  panic("InitialWSError2: INITIAL PAGE WS entry checking failed! Review size of the WS..!!");
				if( ROUNDDOWN(myEnv->__uptr_pws[13].virtual_address,PAGE_SIZE) !=   0x807000)  panic("InitialWSError2: INITIAL PAGE WS entry checking failed! Review size of the WS..!!");
				if( ROUNDDOWN(myEnv->__uptr_pws[14].virtual_address,PAGE_SIZE) !=   0x808000)  panic("InitialWSError2: INITIAL PAGE WS entry checking failed! Review size of the WS..!!");
				if( ROUNDDOWN(myEnv->__uptr_pws[15].virtual_address,PAGE_SIZE) !=   0x809000)  panic("InitialWSError2: INITIAL PAGE WS entry checking failed! Review size of the WS..!!");
				if( ROUNDDOWN(myEnv->__uptr_pws[16].virtual_address,PAGE_SIZE) !=   0x80A000)  panic("InitialWSError2: INITIAL PAGE WS entry checking failed! Review size of the WS..!!");
				if( ROUNDDOWN(myEnv->__uptr_pws[17].virtual_address,PAGE_SIZE) !=   0x80B000)  panic("InitialWSError2: INITIAL PAGE WS entry checking failed! Review size of the WS..!!");
				if( ROUNDDOWN(myEnv->__uptr_pws[18].virtual_address,PAGE_SIZE) !=   0x80C000)  panic("InitialWSError2: INITIAL PAGE WS entry checking failed! Review size of the WS..!!");
				if( ROUNDDOWN(myEnv->__uptr_pws[19].virtual_address,PAGE_SIZE) !=   0x80D000)  panic("InitialWSError2: INITIAL PAGE WS entry checking failed! Review size of the WS..!!");
				if( ROUNDDOWN(myEnv->__uptr_pws[20].virtual_address,PAGE_SIZE) !=   0x80E000)  panic("InitialWSError2: INITIAL PAGE WS entry checking failed! Review size of the WS..!!");
				if( ROUNDDOWN(myEnv->__uptr_pws[21].virtual_address,PAGE_SIZE) !=   0x80F000)  panic("InitialWSError2: INITIAL PAGE WS entry checking failed! Review size of the WS..!!");
				if( ROUNDDOWN(myEnv->__uptr_pws[22].virtual_address,PAGE_SIZE) !=   0x810000)  panic("InitialWSError2: INITIAL PAGE WS entry checking failed! Review size of the WS..!!");
				if( ROUNDDOWN(myEnv->__uptr_pws[23].virtual_address,PAGE_SIZE) !=   0xeebfd000)  panic("InitialWSError2: INITIAL PAGE WS entry checking failed! Review size of the WS..!!");
				if( ROUNDDOWN(myEnv->__uptr_pws[24].virtual_address,PAGE_SIZE) !=   0xee7fe000)  panic("InitialWSError2: INITIAL PAGE WS entry checking failed! Review size of the WS..!!");
				if( myEnv->page_last_WS_index !=  0)  										panic("InitialWSError2: INITIAL PAGE WS last index checking failed! Review size of the WS..!!");
			}
			 */

			cprintf("Checking PAGE FIFO algorithm... \n");
			{
				if( ROUNDDOWN(myEnv->__uptr_pws[0].virtual_address,PAGE_SIZE) !=   0x200000)  	panic("InitialWSError2: INITIAL PAGE WS entry checking failed! Review size of the WS..!!");
				if( ROUNDDOWN(myEnv->__uptr_pws[1].virtual_address,PAGE_SIZE) !=   0x201000)  panic("InitialWSError2: INITIAL PAGE WS entry checking failed! Review size of the WS..!!");
				if( ROUNDDOWN(myEnv->__uptr_pws[2].virtual_address,PAGE_SIZE) !=   0x202000)  panic("InitialWSError2: INITIAL PAGE WS entry checking failed! Review size of the WS..!!");
				if( ROUNDDOWN(myEnv->__uptr_pws[3].virtual_address,PAGE_SIZE) !=   0x203000)  panic("InitialWSError2: INITIAL PAGE WS entry checking failed! Review size of the WS..!!");
				if( ROUNDDOWN(myEnv->__uptr_pws[4].virtual_address,PAGE_SIZE) !=   0x204000)  panic("InitialWSError2: INITIAL PAGE WS entry checking failed! Review size of the WS..!!");
				if( ROUNDDOWN(myEnv->__uptr_pws[5].virtual_address,PAGE_SIZE) !=   0x205000)  panic("InitialWSError2: INITIAL PAGE WS entry checking failed! Review size of the WS..!!");
				if( ROUNDDOWN(myEnv->__uptr_pws[6].virtual_address,PAGE_SIZE) !=   0x800000)  panic("InitialWSError2: INITIAL PAGE WS entry checking failed! Review size of the WS..!!");
				if( ROUNDDOWN(myEnv->__uptr_pws[7].virtual_address,PAGE_SIZE) !=   0x801000)  panic("InitialWSError2: INITIAL PAGE WS entry checking failed! Review size of the WS..!!");
				if( ROUNDDOWN(myEnv->__uptr_pws[8].virtual_address,PAGE_SIZE) !=   0x802000)  panic("InitialWSError2: INITIAL PAGE WS entry checking failed! Review size of the WS..!!");
				if( ROUNDDOWN(myEnv->__uptr_pws[9].virtual_address,PAGE_SIZE) !=   0x803000)  panic("InitialWSError2: INITIAL PAGE WS entry checking failed! Review size of the WS..!!");
				if( ROUNDDOWN(myEnv->__uptr_pws[10].virtual_address,PAGE_SIZE) !=   0x804000)  panic("InitialWSError2: INITIAL PAGE WS entry checking failed! Review size of the WS..!!");
				if( ROUNDDOWN(myEnv->__uptr_pws[11].virtual_address,PAGE_SIZE) !=   0x805000)  panic("InitialWSError2: INITIAL PAGE WS entry checking failed! Review size of the WS..!!");
				if( ROUNDDOWN(myEnv->__uptr_pws[12].virtual_address,PAGE_SIZE) !=   0x806000)  panic("InitialWSError2: INITIAL PAGE WS entry checking failed! Review size of the WS..!!");
				if( ROUNDDOWN(myEnv->__uptr_pws[13].virtual_address,PAGE_SIZE) !=   0x807000)  panic("InitialWSError2: INITIAL PAGE WS entry checking failed! Review size of the WS..!!");
				if( ROUNDDOWN(myEnv->__uptr_pws[14].virtual_address,PAGE_SIZE) !=   0x808000)  panic("InitialWSError2: INITIAL PAGE WS entry checking failed! Review size of the WS..!!");
				if( ROUNDDOWN(myEnv->__uptr_pws[15].virtual_address,PAGE_SIZE) !=   0x809000)  panic("InitialWSError2: INITIAL PAGE WS entry checking failed! Review size of the WS..!!");
				if( ROUNDDOWN(myEnv->__uptr_pws[16].virtual_address,PAGE_SIZE) !=   0x80A000)  panic("InitialWSError2: INITIAL PAGE WS entry checking failed! Review size of the WS..!!");
				if( ROUNDDOWN(myEnv->__uptr_pws[17].virtual_address,PAGE_SIZE) !=   0x80B000)  panic("InitialWSError2: INITIAL PAGE WS entry checking failed! Review size of the WS..!!");
				if( ROUNDDOWN(myEnv->__uptr_pws[18].virtual_address,PAGE_SIZE) !=   0x80C000)  panic("InitialWSError2: INITIAL PAGE WS entry checking failed! Review size of the WS..!!");
				if( ROUNDDOWN(myEnv->__uptr_pws[19].virtual_address,PAGE_SIZE) !=   0x80D000)  panic("InitialWSError2: INITIAL PAGE WS entry checking failed! Review size of the WS..!!");
				if( ROUNDDOWN(myEnv->__uptr_pws[20].virtual_address,PAGE_SIZE) !=   0x80E000)  panic("InitialWSError2: INITIAL PAGE WS entry checking failed! Review size of the WS..!!");
				if( ROUNDDOWN(myEnv->__uptr_pws[21].virtual_address,PAGE_SIZE) !=   0x80F000)  panic("InitialWSError2: INITIAL PAGE WS entry checking failed! Review size of the WS..!!");
				if( ROUNDDOWN(myEnv->__uptr_pws[22].virtual_address,PAGE_SIZE) !=   0x810000)  panic("InitialWSError2: INITIAL PAGE WS entry checking failed! Review size of the WS..!!");
				if( ROUNDDOWN(myEnv->__uptr_pws[23].virtual_address,PAGE_SIZE) !=   0x811000)  panic("InitialWSError2: INITIAL PAGE WS entry checking failed! Review size of the WS..!!");
				if( ROUNDDOWN(myEnv->__uptr_pws[24].virtual_address,PAGE_SIZE) !=   0xeebfd000)  panic("InitialWSError2: INITIAL PAGE WS entry checking failed! Review size of the WS..!!");
				if( ROUNDDOWN(myEnv->__uptr_pws[25].virtual_address,PAGE_SIZE) !=   0xee7fe000)  panic("InitialWSError2: INITIAL PAGE WS entry checking failed! Review size of the WS..!!");
				if( myEnv->page_last_WS_index !=  0)  										panic("InitialWSError2: INITIAL PAGE WS last index checking failed! Review size of the WS..!!");
			}

			//=========================================================//
			//Clear the FFL
			sys_clear_ffl();
			//=========================================================//

			//NOW: it should take from WS

			//Writing (Modified) after freeing the entire FFL:
			//	3 frames should be allocated (stack page, mem table, page file table)
			*ptr4 = garbage2 ;
			//always use pages at 0x801000 and 0x804000
			//			if (garbage4 != *ptr) panic("test failed!");
			//			if (garbage5 != *ptr2) panic("test failed!");

			garbage4 = *ptr ;
			garbage5 = *ptr2 ;

			//Writing (Modified) after freeing the entire FFL:
			//	4 frames should be allocated (4 stack pages)
			*(ptr4+1*PAGE_SIZE) = 'A';
			*(ptr4+2*PAGE_SIZE) = 'B';
			*(ptr4+3*PAGE_SIZE) = 'C';
			*(ptr4+4*PAGE_SIZE) = 'D';
			/*
			//Check the WS after clock algorithm
			int i;
			for (i = 0 ; i < myEnv->page_WS_max_size; i++)
			{
				if (ROUNDDOWN(myEnv->__uptr_pws[i].virtual_address,PAGE_SIZE) ==  0x200000 ||
						ROUNDDOWN(myEnv->__uptr_pws[i].virtual_address,PAGE_SIZE) ==  0x201000 ||
						ROUNDDOWN(myEnv->__uptr_pws[i].virtual_address,PAGE_SIZE) ==  0x202000 ||
						ROUNDDOWN(myEnv->__uptr_pws[i].virtual_address,PAGE_SIZE) ==  0x203000 ||
						ROUNDDOWN(myEnv->__uptr_pws[i].virtual_address,PAGE_SIZE) ==  0x204000 ||
						ROUNDDOWN(myEnv->__uptr_pws[i].virtual_address,PAGE_SIZE) ==  0x205000 ||
						ROUNDDOWN(myEnv->__uptr_pws[i].virtual_address,PAGE_SIZE) ==  0x802000)
					panic("test failed! either wrong victim or victim is not removed from WS");
			}
			 */
			//Check the WS after FIFO algorithm
			if (garbage4 != *ptr || garbage5 != *ptr2) panic("test failed!");

			//There should be two empty locations that are freed for the two tables (mem table and page file table)
			int numOfEmptyLocs = 0 ;
			for (int i = 0 ; i < myEnv->page_WS_max_size; i++)
			{
				if (myEnv->__uptr_pws[i].empty)
					numOfEmptyLocs++ ;
			}
			if (numOfEmptyLocs != 2) panic("test failed! either wrong victim or victim is not removed from WS");

			uint32 expectedAddresses[24] = {0x801000,0x802000,0x803000,0x804000,0x805000,0x806000,0x807000,0x808000,0x809000,0x80a000,0x80b000,0x80c000,0x80d000,0x80e000,0x80f000,0x810000,0x811000,
					0xee3f9000,0xee3fa000,0xee3fb000,0xee3fc000,0xee3fd000,0xee7fe000,0xeebfd000} ;

			int numOfFoundedAddresses = 0;
			for (int j = 0; j < 24; j++)
			{
				for (int i = 0 ; i < myEnv->page_WS_max_size; i++)
				{
					if (ROUNDDOWN(myEnv->__uptr_pws[i].virtual_address,PAGE_SIZE) == expectedAddresses[j])
					{
						numOfFoundedAddresses++;
						break;
					}
				}
			}
			if (numOfFoundedAddresses != 24) panic("test failed! either wrong victim or victim is not removed from WS");


		}


		//Check that the values are successfully stored
		for (i = 0 ; i < PAGE_SIZE*4 ; i+=PAGE_SIZE/2)
		{
			//cprintf("i = %x, address = %x, arr[i] = %d\n", i, &(arr[i]), arr[i]);
			if (arr[i] != -1) panic("test failed!");
		}
		if (*ptr3 != arr[PAGE_SIZE*10-1]) panic("test failed!");


		if (testCase ==3)
		{
			//			cprintf("garbage4 = %d, *ptr = %d\n",garbage4, *ptr);
			if (garbage4 != *ptr) panic("test failed!");
			if (garbage5 != *ptr2) panic("test failed!");

			if (*ptr4 != arr[PAGE_SIZE*11-1]) panic("test failed!");
			if (*(ptr4+1*PAGE_SIZE) != 'A') panic("test failed!");
			if (*(ptr4+2*PAGE_SIZE) != 'B') panic("test failed!");
			if (*(ptr4+3*PAGE_SIZE) != 'C') panic("test failed!");
			if (*(ptr4+4*PAGE_SIZE) != 'D') panic("test failed!");
		}
	}

	cprintf("Congratulations!! test freeRAM (Scenario# %d) completed successfully.\n", testCase);

	return;
}
