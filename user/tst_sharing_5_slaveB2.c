// Test the free of shared variables
#include <inc/lib.h>

void
_main(void)
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
	malloc(0);
	/*=================================================*/

	uint32 *z;
	z = sget(sys_getparentenvid(),"z");
	cprintf("Slave B2 env used z (getSharedObject)\n");
	//To indicate that it's successfully got z
	inctst();

	cprintf("Slave B2 please be patient ...\n");

	//sleep a while to allow the master to remove x & z
	env_sleep(9000);
	//to ensure that the other environments completed successfully
	while (gettst()!=2) ;// panic("test failed");

	int freeFrames = sys_calculate_free_frames() ;

	sfree(z);
	cprintf("Slave B2 env removed z\n");
	int expected = (1+1) + (1+1);
	if ((sys_calculate_free_frames() - freeFrames) !=  expected) panic("wrong free: frames removed not equal 4 !, correct frames to be removed are 4:\nfrom the env: 1 table + 1 frame for z\nframes_storage of z: should be cleared now\n");


	cprintf("Step B completed successfully!!\n\n\n");
	cprintf("Congratulations!! Test of freeSharedObjects [5] completed successfully!!\n\n\n");

	return;
}
