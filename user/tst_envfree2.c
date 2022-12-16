// Scenario that tests environment free run tef2 10 5
#include <inc/lib.h>

void _main(void)
{
	// Testing scenario 2: using dynamic allocation and free
	// Testing removing the allocated pages (static & dynamic) in mem, WS, mapped page tables, env's directory and env's page file

	int freeFrames_before = sys_calculate_free_frames() ;
	int usedDiskPages_before = sys_pf_calculate_allocated_pages() ;
	cprintf("\n---# of free frames before running programs = %d\n", freeFrames_before);

	/*[4] CREATE AND RUN ProcessA & ProcessB*/
	//Create 3 processes
	int32 envIdProcessA = sys_create_env("ef_ms1", (myEnv->page_WS_max_size),(myEnv->SecondListSize), 50);
	int32 envIdProcessB = sys_create_env("ef_ms2", (myEnv->page_WS_max_size)-3,(myEnv->SecondListSize), 50);

	rsttst();

	//Run 2 processes
	sys_run_env(envIdProcessA);
	sys_run_env(envIdProcessB);

	env_sleep(30000);

	//to ensure that the slave environments completed successfully
	while (gettst()!=2) ;// panic("test failed");

	cprintf("\n---# of free frames after running programs = %d\n", sys_calculate_free_frames());

	//Kill the 2 processes
	sys_destroy_env(envIdProcessA);
	sys_destroy_env(envIdProcessB);

	//Checking the number of frames after killing the created environments
	int freeFrames_after = sys_calculate_free_frames() ;
	int usedDiskPages_after = sys_pf_calculate_allocated_pages() ;

	if ((freeFrames_after - freeFrames_before) != 0) {
		cprintf("\n---# of free frames after closing running programs not as before running = %d\ndifference = %d\n",
				freeFrames_after, freeFrames_after - freeFrames_before);
		panic("env_free() does not work correctly... check it again.");
	}

	cprintf("\n---# of free frames after closing running programs returned back to be as before running = %d\n", freeFrames_after);

	cprintf("\n\nCongratulations!! test scenario 2 for envfree completed successfully.\n");
	return;
}
