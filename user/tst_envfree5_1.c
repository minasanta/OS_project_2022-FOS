// Scenario that tests the usage of shared variables
#include <inc/lib.h>

void _main(void)
{
	// Testing removing the shared variables
	// Testing scenario 5_1: Kill ONE program has shared variables and it free it
	int *numOfFinished = smalloc("finishedCount", sizeof(int), 1) ;
	*numOfFinished = 0 ;

	int freeFrames_before = sys_calculate_free_frames() ;
	int usedDiskPages_before = sys_pf_calculate_allocated_pages() ;
	cprintf("\n---# of free frames before running programs = %d\n", freeFrames_before);

	int32 envIdProcessA = sys_create_env("ef_tshr4", 2000,(myEnv->SecondListSize), 50);

	sys_run_env(envIdProcessA);

	while (*numOfFinished != 1) ;
	cprintf("\n---# of free frames after running programs = %d\n", sys_calculate_free_frames());

	sys_destroy_env(envIdProcessA);

	//Checking the number of frames after killing the created environments
	int freeFrames_after = sys_calculate_free_frames() ;
	int usedDiskPages_after = sys_pf_calculate_allocated_pages() ;

	if ((freeFrames_after - freeFrames_before) != 0) {
		cprintf("\n---# of free frames after closing running programs not as before running = %d\n", freeFrames_after);
		panic("env_free() does not work correctly... check it again.");
	}

	cprintf("\n---# of free frames after closing running programs returned back to be as before running = %d\n", freeFrames_after);

	cprintf("\n\nCongratulations!! test scenario 5_1 for envfree completed successfully.\n");
	return;
}
