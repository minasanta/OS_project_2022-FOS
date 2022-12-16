// Test the use of semaphores for critical section & dependency
// Master program: create the semaphores, run slaves and wait them to finish
#include <inc/lib.h>

void
_main(void)
{
	int envID = sys_getenvid();

	sys_createSemaphore("cs1", 1);

	sys_createSemaphore("depend1", 0);

	int id1, id2, id3;
	id1 = sys_create_env("ef_sem1Slave", (myEnv->page_WS_max_size),(myEnv->SecondListSize), 50);
	id2 = sys_create_env("ef_sem1Slave", (myEnv->page_WS_max_size),(myEnv->SecondListSize), 50);
	id3 = sys_create_env("ef_sem1Slave", (myEnv->page_WS_max_size),(myEnv->SecondListSize), 50);
	if (id1 == E_ENV_CREATION_ERROR || id2 == E_ENV_CREATION_ERROR || id3 == E_ENV_CREATION_ERROR)
		panic("NO AVAILABLE ENVs...");

	sys_run_env(id1);
	sys_run_env(id2);
	sys_run_env(id3);

	sys_waitSemaphore(envID, "depend1") ;
	sys_waitSemaphore(envID, "depend1") ;
	sys_waitSemaphore(envID, "depend1") ;

	int sem1val = sys_getSemaphoreValue(envID, "cs1");
	int sem2val = sys_getSemaphoreValue(envID, "depend1");
	if (sem2val == 0 && sem1val == 1)
		cprintf("Test of Semaphores is finished!!\n\n\n");
	else
		cprintf("Error: wrong semaphore value... please review your semaphore code again...");

	int32 parentenvID = sys_getparentenvid();
	if(parentenvID > 0)
	{
		sys_destroy_env(id1);
		sys_destroy_env(id2);
		sys_destroy_env(id3);
		sys_signalSemaphore(parentenvID, "depend0") ;

		//Get the check-finishing counter
//		int *finishedCount = NULL;
//		finishedCount = sget(parentenvID, "finishedCount") ;
//		(*finishedCount)++ ;
	}

	return;
}
