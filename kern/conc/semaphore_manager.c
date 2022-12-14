#include <inc/mmu.h>
#include <inc/error.h>
#include <inc/string.h>
#include <inc/assert.h>
#include <inc/environment_definitions.h>

#include "semaphore_manager.h"
#include <kern/proc/user_environment.h>
#include <kern/cpu/sched.h>
#include <kern/mem/kheap.h>
#include <kern/mem/memory_manager.h>


//==================================================================================//
//============================== GIVEN FUNCTIONS ===================================//
//==================================================================================//

//===============================
// [1] Create "semaphores" array:
//===============================
//Dynamically allocate the "semaphores" array
//initialize the "semaphores" array by 0's and empty = 1
void create_semaphores_array(uint32 numOfSemaphores)
{
#if USE_KHEAP
	MAX_SEMAPHORES = numOfSemaphores ;
	semaphores = (struct Semaphore*) kmalloc(numOfSemaphores*sizeof(struct Semaphore));
	if (semaphores == NULL)
	{
		panic("Kernel runs out of memory\nCan't create the array of semaphores.");
	}
#endif
	for (int i = 0; i < MAX_SEMAPHORES; ++i)
	{
		memset(&(semaphores[i]), 0, sizeof(struct Semaphore));
		semaphores[i].empty = 1;
		LIST_INIT(&(semaphores[i].env_queue));
	}

}


//========================
// [2] Allocate Semaphore:
//========================
//Allocates a new (empty) semaphore object from the "semaphores" array
//Return:
//	a) if succeed:
//		1. allocatedSemaphore (pointer to struct Semaphore) passed by reference
//		2. SempahoreObjectID (its index in the array) as a return parameter
//	b) E_NO_SEMAPHORE if the the array of semaphores is full (i.e. reaches "MAX_SEMAPHORES")
int allocate_semaphore_object(struct Semaphore **allocatedObject)
{
	int32 semaphoreObjectID = -1 ;
	for (int i = 0; i < MAX_SEMAPHORES; ++i)
	{
		if (semaphores[i].empty)
		{
			semaphoreObjectID = i;
			break;
		}
	}

	if (semaphoreObjectID == -1)
	{
		//try to double the size of the "semaphores" array
		#if USE_KHEAP
		{
			semaphores = (struct Semaphore*) krealloc(semaphores, 2*MAX_SEMAPHORES);
			if (semaphores == NULL)
			{
				*allocatedObject = NULL;
				return E_NO_SEMAPHORE;
			}
			else
			{
				semaphoreObjectID = MAX_SEMAPHORES;
				MAX_SEMAPHORES *= 2;
			}
		}
		#else
		{
			panic("Attempt to dynamically allocate space inside kernel while kheap is disabled .. ");
			*allocatedObject = NULL;
			return E_NO_SEMAPHORE;
		}
		#endif
	}

	*allocatedObject = &(semaphores[semaphoreObjectID]);
	semaphores[semaphoreObjectID].empty = 0;

	return semaphoreObjectID;
}

//======================
// [3] Get Semaphore ID:
//======================
//Search for the given semaphore object in the "semaphores" array
//Return:
//	a) if found: SemaphoreObjectID (index of the semaphore object in the array)
//	b) else: E_SEMAPHORE_NOT_EXISTS
int get_semaphore_object_ID(int32 ownerID, char* name)
{
	int i=0;
	for(; i < MAX_SEMAPHORES; ++i)
	{
		if (semaphores[i].empty)
			continue;

		if(semaphores[i].ownerID == ownerID && strcmp(name, semaphores[i].name)==0)
		{
			return i;
		}
	}
	return E_SEMAPHORE_NOT_EXISTS;
}

//====================
// [4] Free Semaphore:
//====================
//delete the semaphore with the given ID from the "semaphores" array
//Return:
//	a) 0 if succeed
//	b) E_SEMAPHORE_NOT_EXISTS if the semaphore is not exists
int free_semaphore_object(uint32 semaphoreObjectID)
{
	if (semaphoreObjectID >= MAX_SEMAPHORES)
		return E_SEMAPHORE_NOT_EXISTS;

	memset(&(semaphores[semaphoreObjectID]), 0, sizeof(struct Semaphore));
	semaphores[semaphoreObjectID].empty = 1;
	LIST_INIT(&(semaphores[semaphoreObjectID].env_queue));

	return 0;
}

//==================================================================================//
//============================ REQUIRED FUNCTIONS ==================================//
//==================================================================================//

//======================
// [1] Create Semaphore:
//======================
int createSemaphore(int32 ownerEnvID, char* semaphoreName, uint32 initialValue)
{
	// TODO: [PROJECT MS3] [SEMAPHORES] createSemaphore
	// your code is here, remove the panic and write your code
	// panic("createSemaphore() is not implemented yet...!!");

	//create new semaphore object and initialize it by the given info (ownerID, name, value)
	//Return:
	//	a) SemaphoreID (its index in the array) if succeed
	//	b) E_SEMAPHORE_EXISTS if the semaphore is already exists
	//	c) E_NO_SEMAPHORE if the the array of semaphores is full
	int semaphore = get_semaphore_object_ID(ownerEnvID, semaphoreName);
	if (semaphore == E_SEMAPHORE_NOT_EXISTS)
	{
		struct Semaphore *allocatedObject;
		int ret = allocate_semaphore_object(&allocatedObject);
		if (ret == E_NO_SEMAPHORE)
			return E_NO_SEMAPHORE;
		semaphores[ret].ownerID = ownerEnvID;
		strcpy(semaphores[ret].name, semaphoreName);
		semaphores[ret].value = initialValue;
		return ret;
	}
	else
	{
		return E_SEMAPHORE_EXISTS;
	}
	//change this "return" according to your answer
}

//============
// [2] Wait():
//============
void waitSemaphore(int32 ownerEnvID, char* semaphoreName)
{
	//TODO: [PROJECT MS3] [SEMAPHORES] waitSemaphore
	// your code is here, remove the panic and write your code
	// panic("waitSemaphore() is not implemented yet...!!");

	struct Env* myenv = curenv; //The calling environment

	// Steps:
	//	1) Get the Semaphore
	int sem = get_semaphore_object_ID(ownerEnvID, semaphoreName);
	//	2) Decrement its value
	semaphores[sem].value--;
	//	3) If negative, block the calling environment "myenv", by
	if (semaphores[sem].value < 0)
	{
	//		a) adding it to semaphore queue		[refer to helper functions in doc]
		enqueue(&semaphores[sem].env_queue, myenv);
	//		b) changing its status to ENV_BLOCKED
		myenv->env_status = ENV_BLOCKED;
	//		c) set curenv with NULL
		curenv = NULL;
	//	4) Call "fos_scheduler()" to continue running the remaining envs
		fos_scheduler();
	}
}

//==============
// [3] Signal():
//==============
void signalSemaphore(int ownerEnvID, char* semaphoreName)
{
	//TODO: [PROJECT MS3] [SEMAPHORES] signalSemaphore
	// your code is here, remove the panic and write your code
	// panic("signalSemaphore() is not implemented yet...!!");

	// Steps:
	//	1) Get the Semaphore
	int sem = get_semaphore_object_ID(ownerEnvID, semaphoreName);
	//	2) Increment its value
	semaphores[sem].value++;
	//	3) If less than or equal 0, release a blocked environment, by
	if (semaphores[sem].value <= 0)
	{
	//		a) removing it from semaphore queue		[refer to helper functions in doc]
		struct Env* env = dequeue(&semaphores[sem].env_queue);
	//		b) adding it to ready queue				[refer to helper functions in doc]
		sched_insert_ready(env);
	//		c) changing its status to ENV_READY
		env->env_status = ENV_READY;
	}
}

