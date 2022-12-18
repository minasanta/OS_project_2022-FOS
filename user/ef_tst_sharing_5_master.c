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

	int envID = sys_getenvid();

	int32 envIdSlave1, envIdSlave2, envIdSlaveB1, envIdSlaveB2;

	cprintf("STEP A: checking free of shared object using 2 environments... \n");
	{
		uint32 *x;
		envIdSlave1 = sys_create_env("ef_tshr5slave", (myEnv->page_WS_max_size),(myEnv->SecondListSize), 50);
		envIdSlave2 = sys_create_env("ef_tshr5slave", (myEnv->page_WS_max_size),(myEnv->SecondListSize), 50);

		int freeFrames = sys_calculate_free_frames() ;
		x = smalloc("x", PAGE_SIZE, 1);
		cprintf("Master env created x (1 page) \n");
		if (x != (uint32*)USER_HEAP_START) panic("Returned address is not correct. check the setting of it and/or the updating of the shared_mem_free_address");
		if ((freeFrames - sys_calculate_free_frames()) !=  1+1+2) panic("Wrong allocation: make sure that you allocate the required space in the user environment and add its frames to frames_storage");

		//to check that the slave environments completed successfully
		rsttst();

		sys_run_env(envIdSlave1);
		sys_run_env(envIdSlave2);

		cprintf("please be patient ...\n");
		env_sleep(3000);

		//to ensure that the slave environments completed successfully
		while (gettst()!=2) ;// panic("test failed");

		freeFrames = sys_calculate_free_frames() ;
		sfree(x);
		cprintf("Master env removed x (1 page) \n");
		int diff = (sys_calculate_free_frames() - freeFrames);
		if ( diff != (1+1+2)) panic("Wrong free: revise your freeSharedObject logic. diff = %d, expected = %d\n", diff, (1+1+2));
	}
	cprintf("Step A is finished!!\n\n\n");

	cprintf("STEP B: checking free of 2 shared objects ... \n");
	{
		uint32 *x, *z ;
		envIdSlaveB1 = sys_create_env("ef_tshr5slaveB1", (myEnv->page_WS_max_size),(myEnv->SecondListSize), 50);
		envIdSlaveB2 = sys_create_env("ef_tshr5slaveB2", (myEnv->page_WS_max_size), (myEnv->SecondListSize),50);

		z = smalloc("z", PAGE_SIZE, 1);
		cprintf("Master env created z (1 page) \n");

		x = smalloc("x", PAGE_SIZE, 1);
		cprintf("Master env created x (1 page) \n");

		rsttst();

		sys_run_env(envIdSlaveB1);
		sys_run_env(envIdSlaveB2);

		//env_sleep(4000); //give slaves time to
		//to ensure that the slaves catch the shared object before removal
		while (gettst()!=2) ;// panic("test failed");

		int freeFrames = sys_calculate_free_frames() ;

		sfree(z);
		inctst();
		cprintf("Master env removed z\n");

		sfree(x);
		inctst();
		cprintf("Master env removed x\n");

		int diff = (sys_calculate_free_frames() - freeFrames);

		if (diff !=  1) panic("Wrong free: frames removed not equal 1 !, correct frames to be removed are 1:\nfrom the env: 1 table\nframes_storage of z & x: should NOT cleared yet (still in use!)\n");

		int* finish_children = smalloc("finish_children", sizeof(int), 1);
		*finish_children = 0;

		//to ensure that the other environments completed successfully
		while (gettst()!=6) ;// panic("test failed");

		//To indicate that it's completed successfully
		inctst();

		if (sys_getparentenvid() > 0) {
			while(*finish_children != 1);
			cprintf("done\n");
			sys_destroy_env(envIdSlave1);
			sys_destroy_env(envIdSlave2);
			sys_destroy_env(envIdSlaveB1);
			sys_destroy_env(envIdSlaveB2);

			int *finishedCount = NULL;
			finishedCount = sget(sys_getparentenvid(), "finishedCount") ;
			(*finishedCount)++ ;
		}
	}


	return;
}
