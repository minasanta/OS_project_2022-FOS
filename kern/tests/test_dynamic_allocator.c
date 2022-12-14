/*
 * test_lists_managment.c

 *
 *  Created on: Oct 6, 2022
 *      Author: HP
 */
#include <inc/queue.h>
#include <inc/string.h>
#include <inc/assert.h>
#include <inc/dynamic_allocator.h>

//NOTE: ALL tests in this file shall work with USE_KHEAP = 0 & STATIC_MEMBLOCK_ALLOC = 1

//=============//
int check_list_data(struct MemBlock_List* list, uint32* blocksSVAs, uint32* blocksSizes, uint32* size, uint32 actualSize);
int check_avaliable_list_data(uint32* size);
void show_list_content(struct MemBlock_List* list);
//=============//

#define Mega  (1024*1024)
#define kilo (1024)

void test_initialize_MemBlocksList()
{
	uint32 numOfBlocks = 5;
	initialize_MemBlocksList(numOfBlocks);
	//Check 1: Check number of nodes in AvailableMemBlocksList
	if (LIST_SIZE(&(AvailableMemBlocksList)) != numOfBlocks)
	{
		panic("initialize_MemBlocksList: Allocating wrong size for the AvailableMemBlocksList %d\n", LIST_SIZE(&(AvailableMemBlocksList)));
		//assert(LIST_SIZE(&AvailableMemBlocksList) == numOfBlocks);
	}

	//Check 2: Check number of nodes in AllocMemBlocksList
	if (LIST_SIZE(&(AllocMemBlocksList)) != 0)
	{
		panic("initialize_MemBlocksList: Allocating wrong size for the AllocMemBlocksList %d\n", LIST_SIZE(&(AllocMemBlocksList)));
	}

	//Check 3: Check number of nodes in AvailableMemBlocksList
	if (LIST_SIZE(&(FreeMemBlocksList)) != 0)
	{
		panic("initialize_MemBlocksList: Allocating wrong size for the FreeMemBlocksList %d\n", LIST_SIZE(&(FreeMemBlocksList)));
	}

	//Check 4: Check the existance of each node and its data in AvailableMemBlocksList
	struct MemBlock* node = NULL;
	uint32 size = 0;
	LIST_FOREACH(node, &AvailableMemBlocksList)
	{
		size++;
		cprintf("Node data: sva = %x, size = %d\n", node->sva, node->size);
	}
	if(LIST_SIZE(&(AvailableMemBlocksList)) != size)
		panic("initialize_MemBlocksList: Some nodes are not created in the initialize_MemBlocksList\n");
	else
		cprintf("Congratulations!! test initialize_MemBlocksList completed successfully.\n");
}

void test_insert_sorted_allocList()
{
	uint32 numOfBlocks = 5;
	initialize_MemBlocksList(numOfBlocks);
	LIST_INIT(&AllocMemBlocksList);
	LIST_INIT(&FreeMemBlocksList);

	uint32 blocksToInsertSVAs[5]={0,0,0,0,0};
	uint32 blocksToInsertSizes[5]={0,0,0,0,0};
	uint32 size = 0;
	uint32 actualSize = 0;


	/*INSERT Scenario 1: Insert BLOCK_1 in the empty AllocMemBlocksList*/
	//BLOCK_1 sva = 8K
	blocksToInsertSVAs[1] = 0x0 + (8*kilo);
	blocksToInsertSizes[1] = (4*kilo);
	struct MemBlock blockToInsert1;
	blockToInsert1.sva = blocksToInsertSVAs[1];
	blockToInsert1.size = blocksToInsertSizes[1];

	insert_sorted_allocList(&blockToInsert1);

	//Check 1: Check size of the AllocMemBlocksList
	actualSize = 1;
	if (LIST_SIZE(&(AllocMemBlocksList)) != actualSize) panic("insert_sorted: WRONG ALLOCATION - wrong size for the AllocMemBlocksList.");

	//Check 2: Check size of the AvailableMemBlocksList
	if (LIST_SIZE(&(AvailableMemBlocksList)) != numOfBlocks) panic("insert_sorted: WRONG ALLOCATION - wrong size for the AvailableMemBlocksList.");

	//Check 3: Check the allocated list content
	int chk = check_list_data(&AllocMemBlocksList, blocksToInsertSVAs, blocksToInsertSizes, &size, actualSize);
	if(chk != 1) panic("insert_sorted: WRONG ALLOCATION .. AllocMemBlocksList content is not correct.");
	if(size != actualSize) panic("insert_sorted: WRONG ALLOCATION .. AllocMemBlocksList size is not correct.");

	//====================================================================//
	/*INSERT Scenario 2: Insert BLOCK_2 in the AllocMemBlocksList with sva greater than BLOCK_1*/
	//BLOCK_2 sva = 28K
	blocksToInsertSVAs[4] = (28*kilo);
	blocksToInsertSizes[4] = (2*kilo);
	struct MemBlock blockToInsert2;
	blockToInsert2.sva = blocksToInsertSVAs[4];
	blockToInsert2.size = blocksToInsertSizes[4];

	insert_sorted_allocList(&blockToInsert2);

	//Check 1: Check size of the AllocMemBlocksList
	actualSize = 2;
	if (LIST_SIZE(&(AllocMemBlocksList)) != actualSize) panic("insert_sorted: WRONG ALLOCATION - wrong size for the AllocMemBlocksList.");

	//Check 2: Check size of the AvailableMemBlocksList
	if (LIST_SIZE(&(AvailableMemBlocksList)) != numOfBlocks) panic("insert_sorted: WRONG ALLOCATION - wrong size for the AvailableMemBlocksList.");

	//Check 3: Check the allocated list content
	size = 0;
	chk = check_list_data(&AllocMemBlocksList, blocksToInsertSVAs, blocksToInsertSizes, &size, actualSize);
	if(chk != 1) panic("insert_sorted: WRONG ALLOCATION .. AllocMemBlocksList content is not correct.");
	if(size != actualSize) panic("insert_sorted: WRONG ALLOCATION .. AllocMemBlocksList size is not correct.");

	//====================================================================//
	/*INSERT Scenario 3: Insert BLOCK_3 in the AllocMemBlocksList with sva smaller than BLOCK_2*/
	//BLOCK_3 sva = 16K
	blocksToInsertSVAs[2] = (16*kilo);
	blocksToInsertSizes[2] = (8*kilo);
	struct MemBlock blockToInsert3;
	blockToInsert3.sva = blocksToInsertSVAs[2];
	blockToInsert3.size = blocksToInsertSizes[2];

	insert_sorted_allocList(&blockToInsert3);

	//Check 1: Check size of the AllocMemBlocksList
	actualSize = 3;
	if (LIST_SIZE(&(AllocMemBlocksList)) != actualSize) panic("insert_sorted: WRONG ALLOCATION - wrong size for the AllocMemBlocksList.");

	//Check 2: Check size of the AvailableMemBlocksList
	if (LIST_SIZE(&(AvailableMemBlocksList)) != numOfBlocks) panic("insert_sorted: WRONG ALLOCATION - wrong size for the AvailableMemBlocksList\n");

	//Check 3: Check the allocated list content
	size = 0;
	chk = check_list_data(&AllocMemBlocksList, blocksToInsertSVAs, blocksToInsertSizes, &size, actualSize);
	if(chk != 1) panic("insert_sorted: WRONG ALLOCATION .. AllocMemBlocksList content is not correct.");
	if(size != actualSize) panic("insert_sorted: WRONG ALLOCATION .. AllocMemBlocksList size is not correct.");

	//====================================================================//
	//INSERT Scenario 4: Insert BLOCK_4 in the AllocMemBlocksList with sva smaller than ALL (SHALL BE INSERTED AS HEAD)
	//BLOCK_4 sva = 0x0
	blocksToInsertSVAs[0] = (0x0);
	blocksToInsertSizes[0] = (8*kilo);
	struct MemBlock blockToInsert4;
	blockToInsert4.sva = blocksToInsertSVAs[0];
	blockToInsert4.size = blocksToInsertSizes[0];
	insert_sorted_allocList(&blockToInsert4);

	//Check 1: Check size of the AllocMemBlocksList
	actualSize = 4;
	if (LIST_SIZE(&(AllocMemBlocksList)) != actualSize) panic("insert_sorted: WRONG ALLOCATION - wrong size for the AllocMemBlocksList.");

	//Check 2: Check size of the AvailableMemBlocksList
	if (LIST_SIZE(&(AvailableMemBlocksList)) != numOfBlocks) panic("insert_sorted: WRONG ALLOCATION - wrong size for the AvailableMemBlocksList.");

	//Check 3: Check the allocated list content
	size = 0;
	chk = check_list_data(&AllocMemBlocksList, blocksToInsertSVAs, blocksToInsertSizes, &size, actualSize);
	if(chk != 1) panic("insert_sorted: WRONG ALLOCATION .. AllocMemBlocksList content is not correct.");
	if(size != actualSize) panic("insert_sorted: WRONG ALLOCATION .. AllocMemBlocksList size is not correct.");

	//====================================================================//
	//INSERT Scenario 5: Insert BLOCK_5 in the AllocMemBlocksList with sva not greater than the first block only
	//BLOCK_5 sva = 24K
	blocksToInsertSVAs[3] = 24*kilo;
	blocksToInsertSizes[3] = (4*kilo);
	struct MemBlock blockToInsert5;
	blockToInsert5.sva = blocksToInsertSVAs[3];
	blockToInsert5.size = blocksToInsertSizes[3];
	insert_sorted_allocList(&blockToInsert5);

	//Check 1: Check size of the AllocMemBlocksList
	actualSize = 5;
	if (LIST_SIZE(&(AllocMemBlocksList)) != actualSize) panic("insert_sorted: WRONG ALLOCATION - wrong size for the AllocMemBlocksList.");

	//Check 2: Check size of the AvailableMemBlocksList
	if (LIST_SIZE(&(AvailableMemBlocksList)) != numOfBlocks) panic("insert_sorted: WRONG ALLOCATION - wrong size for the AvailableMemBlocksList.");

	//Check 3: Check the allocated list content
	size = 0;
	chk = check_list_data(&AllocMemBlocksList, blocksToInsertSVAs, blocksToInsertSizes, &size, actualSize);
	if(chk != 1) panic("insert_sorted: WRONG ALLOCATION .. AllocMemBlocksList content is not correct.");
	if(size != actualSize) panic("insert_sorted: WRONG ALLOCATION .. AllocMemBlocksList size is not correct.");
	//====================================================================//
	//====================================================================//
	//show_list_content(&AllocMemBlocksList);

	cprintf("Congratulations!! test insert_sorted completed successfully.\n");
}

void test_find_block()
{
	uint32 numOfBlocks = 6;
	initialize_MemBlocksList(numOfBlocks);
	LIST_INIT(&FreeMemBlocksList);

	//Fill AllocMemBlocksList with multiple blocks
	#define numOfAllocBlocks  6
	uint32 BlocksSVAs[numOfAllocBlocks] = {0x0, 0x2000, 0x4000, 0x7000, 0x19000, 0x1A400}; //0KB, 8KB, 16KB, 28KB, 100KB, 105KB
	uint32 BlocksSizes[numOfAllocBlocks] = {3*kilo, 8*kilo, 1*kilo, 9*kilo, 4*kilo, 2*Mega} ;

	LIST_INIT(&AllocMemBlocksList);
	struct MemBlock blockToInsert[numOfAllocBlocks];
	for (int i = 0; i < numOfAllocBlocks; ++i)
	{
		blockToInsert[i].sva = BlocksSVAs[i];
		blockToInsert[i].size = BlocksSizes[i];
		if (i == 0)
			LIST_INSERT_HEAD(&AllocMemBlocksList, &(blockToInsert[i]));
		else
			LIST_INSERT_TAIL(&AllocMemBlocksList, &(blockToInsert[i]));
	}

	/*FIND Scenario 1: Search for a block that doesn't exist (Block with SVA = 4KB)*/
	struct MemBlock * blockToFind = find_block(&AllocMemBlocksList, 0x1000);
	if(blockToFind != NULL)
		panic("find_block: WRONG SEARCH - The block you searched for doesn't exist, however find_block find it.");

	/*FIND Scenario 2: Search for a block exist @ the beginning (Block with SVA = 0KB)*/
	blockToFind = find_block(&AllocMemBlocksList, 0x0);
	if(blockToFind == NULL || blockToFind->size != 3*kilo || blockToFind->sva != 0x0)
		panic("find_block: WRONG SEARCH - You find wrong block.");

	/*FIND Scenario 3: Search for a block exist @ the mid (Block with SVA = 0x7000)*/
	blockToFind = find_block(&AllocMemBlocksList, 0x7000);
	if(blockToFind == NULL || blockToFind->size != 9*kilo || blockToFind->sva != 0x7000)
		panic("find_block: WRONG SEARCH - You find wrong block.");

	/*FIND Scenario 4: Search for a block exist @ the end (Block with SVA = 0x7000)*/
	blockToFind = find_block(&AllocMemBlocksList, 0x1A400);
	if(blockToFind == NULL || blockToFind->size != 2*Mega || blockToFind->sva != 0x1A400)
		panic("find_block: WRONG SEARCH - You find wrong block.");

	//Check size of AvailableMemBlocksList & FreeMemBlocksList
	if (LIST_SIZE(&(AvailableMemBlocksList)) != numOfBlocks || LIST_SIZE(&(FreeMemBlocksList)) != 0 )
	{
		panic("find_block: AvailableMemBlocksList and FreeMemBlocksList are affected and this shall not occurred.");
	}

	//show_list_content(&AllocMemBlocksList);
	cprintf("Congratulations!! test find_block completed successfully.\n");
}

void test_alloc_block_FF()
{
	uint32 numOfBlocks = 10;
	initialize_MemBlocksList(numOfBlocks);
	LIST_INIT(&AllocMemBlocksList);

	//Fill AllocMemBlocksList with multiple blocks
	#define numOfFreeBlocks_FF  8
	uint32 BlocksSVAs[numOfFreeBlocks_FF] = {0x0, 0x2000, 0x4000, 0x7000, 0x19000, 0x1A400, 0x21B400, 0x31B400}; //0KB, 8KB, 16KB, 28KB, 100KB, 105KB, 2,157KB, 3,181KB
	uint32 BlocksSizes[numOfFreeBlocks_FF] = {3*kilo, 8*kilo, 1*kilo, 9*kilo, 4*kilo, 2*Mega, 1*Mega, 5*kilo} ;

	LIST_INIT(&FreeMemBlocksList);
	struct MemBlock freeBlocks[numOfFreeBlocks_FF];
	for (int i = 0; i < numOfFreeBlocks_FF; ++i)
	{
		freeBlocks[i].sva = BlocksSVAs[i];
		freeBlocks[i].size = BlocksSizes[i];
		if (i == 0)
			LIST_INSERT_HEAD(&FreeMemBlocksList, &(freeBlocks[i]));
		else
			LIST_INSERT_TAIL(&FreeMemBlocksList, &(freeBlocks[i]));
	}

	//====================================================================//
	/*FF ALLOC Scenario 1: Try to allocate a block with a size greater than the size of any existing free block*/
	struct MemBlock * blockToAlloc = alloc_block_FF(3*Mega);

	//Check returned block content
	if(blockToAlloc != NULL) panic("alloc_block_FF: WRONG FF ALLOC - alloc_block_FF find a block instead no existing free blocks with the given size.");

	//Check size of AvailableMemBlocksList, FreeMemBlocksList & AllocMemBlocksList
	if (LIST_SIZE(&(AvailableMemBlocksList)) != numOfBlocks || LIST_SIZE(&(FreeMemBlocksList)) != numOfFreeBlocks_FF || LIST_SIZE(&(AllocMemBlocksList)) != 0)
		panic("find_block: AvailableMemBlocksList, FreeMemBlocksList & AllocMemBlocksList are affected and this shall not occurred.");

	//====================================================================//
	/*FF ALLOC Scenario 2: Try to allocate a block with a size equal to the size of the first existing free block*/
	blockToAlloc = alloc_block_FF(9*kilo);

	//Check returned block content
	if(blockToAlloc == NULL || blockToAlloc->size != 9*kilo || blockToAlloc->sva != 0x7000)
		panic("alloc_block_FF: WRONG FF ALLOC - alloc_block_FF find a wrong block.");

	//Check size of AvailableMemBlocksList, FreeMemBlocksList & AllocMemBlocksList
	if (LIST_SIZE(&(AvailableMemBlocksList)) != numOfBlocks || LIST_SIZE(&(FreeMemBlocksList)) != numOfFreeBlocks_FF-1 || LIST_SIZE(&(AllocMemBlocksList)) != 0)
		panic("alloc_block_FF: Wrong sizes for AvailableMemBlocksList, FreeMemBlocksList and AllocMemBlocksList.");

	//Check FreeMemBlocksList content
	uint32 size = 0;
	uint32 actualSize = numOfFreeBlocks_FF-1;
	BlocksSVAs[3] = 0;
	BlocksSizes[3] = 0;
	int chk = check_list_data(&FreeMemBlocksList, BlocksSVAs, BlocksSizes, &size, actualSize);
	if(chk != 1) panic("alloc_block_FF: WRONG FF ALLOC .. FreeMemBlocksList content is not correct.");
	if(size != actualSize) panic("alloc_block_FF: WRONG FF ALLOC .. FreeMemBlocksList size is not correct.");

	//====================================================================//
	/*FF ALLOC Scenario 3: Try to allocate a block with a size smaller than the size of the first existing free block*/
	blockToAlloc = alloc_block_FF(1*Mega);

	//Check returned block content
	if(blockToAlloc == NULL || blockToAlloc->size != 1*Mega || blockToAlloc->sva != 0x1A400)
		panic("alloc_block_FF: WRONG FF ALLOC - alloc_block_FF find a wrong block.");

	//Check size of AvailableMemBlocksList, FreeMemBlocksList & AllocMemBlocksList
	if (LIST_SIZE(&(AvailableMemBlocksList)) != numOfBlocks-1 || LIST_SIZE(&(FreeMemBlocksList)) != numOfFreeBlocks_FF-1 || LIST_SIZE(&(AllocMemBlocksList)) != 0)
		panic("alloc_block_FF: Wrong sizes for AvailableMemBlocksList, FreeMemBlocksList and AllocMemBlocksList.");

	//Check FreeMemBlocksList content
	size = 0;
	actualSize = numOfFreeBlocks_FF-1;
	BlocksSVAs[5] = 0x11A400;  //105KB + 1Mega = (105*1024)+(1024*1024)
	BlocksSizes[5] = 1*Mega;
	chk = check_list_data(&FreeMemBlocksList, BlocksSVAs, BlocksSizes, &size, actualSize);
	if(chk != 1) panic("alloc_block_FF: WRONG FF ALLOC .. FreeMemBlocksList content is not correct.");
	if(size != actualSize) panic("alloc_block_FF: WRONG FF ALLOC .. FreeMemBlocksList size is not correct.");

	//====================================================================//
	//show_list_content(&FreeMemBlocksList);
	cprintf("Congratulations!! test alloc_block_FF completed successfully.\n");
}

void test_alloc_block_BF()
{
	uint32 numOfBlocks = 10;
	initialize_MemBlocksList(numOfBlocks);
	LIST_INIT(&AllocMemBlocksList);

	//Fill AllocMemBlocksList with multiple blocks
	#define numOfFreeBlocks_BF 7
	uint32 BlocksSVAs[numOfFreeBlocks_BF] = {0x0, 0x2000, 0x4000, 0x7000, 0x19000, 0x1A400, 0x21B400}; //0KB, 8KB, 16KB, 28KB, 100KB, 105KB, 2,157KB
	uint32 BlocksSizes[numOfFreeBlocks_BF] = {2*kilo, 8*kilo, 1*kilo, 9*kilo, 4*kilo, 2*Mega, 1*Mega} ;

	LIST_INIT(&FreeMemBlocksList);
	struct MemBlock freeBlocks[numOfFreeBlocks_BF];
	for (int i = 0; i < numOfFreeBlocks_BF; ++i)
	{
		freeBlocks[i].sva = BlocksSVAs[i];
		freeBlocks[i].size = BlocksSizes[i];
		if (i == 0)
			LIST_INSERT_HEAD(&FreeMemBlocksList, &(freeBlocks[i]));
		else
			LIST_INSERT_TAIL(&FreeMemBlocksList, &(freeBlocks[i]));
	}

	//====================================================================//
	/*BF ALLOC Scenario 1: Try to allocate a block with a size greater than the size of any existing free block*/
	struct MemBlock * blockToAlloc = alloc_block_BF(4*Mega);

	//Check returned block content
	if(blockToAlloc != NULL) panic("alloc_block_BF: WRONG BF ALLOC - alloc_block_BF find a block instead no existing free blocks with the given size.");

	//Check size of AvailableMemBlocksList, FreeMemBlocksList & AllocMemBlocksList
	if (LIST_SIZE(&(AvailableMemBlocksList)) != numOfBlocks || LIST_SIZE(&(FreeMemBlocksList)) != numOfFreeBlocks_BF || LIST_SIZE(&(AllocMemBlocksList)) != 0)
		panic("find_block: AvailableMemBlocksList, FreeMemBlocksList & AllocMemBlocksList are affected and this shall not occurred.");

	//====================================================================//
	/*BF ALLOC Scenario 2: Try to allocate a block with a size equal to the size of the one existing free blocks (NOT THE FIRST BLOCK FIT)*/
	blockToAlloc = alloc_block_BF(1*Mega);

	//Check returned block content
	if(blockToAlloc == NULL || blockToAlloc->size != 1*Mega || blockToAlloc->sva != 0x21B400)
		panic("alloc_block_BF: WRONG BF ALLOC - alloc_block_BF find a wrong block.");

	//Check size of AvailableMemBlocksList, FreeMemBlocksList & AllocMemBlocksList
	if (LIST_SIZE(&(AvailableMemBlocksList)) != numOfBlocks || LIST_SIZE(&(FreeMemBlocksList)) != numOfFreeBlocks_BF-1 || LIST_SIZE(&(AllocMemBlocksList)) != 0)
		panic("alloc_block_BF: Wrong sizes for AvailableMemBlocksList, FreeMemBlocksList and AllocMemBlocksList.");

	//Check FreeMemBlocksList content
	uint32 size = 0;
	uint32 actualSize = numOfFreeBlocks_BF-1;
	BlocksSVAs[6] = 0;
	BlocksSizes[6] = 0;
	int chk = check_list_data(&FreeMemBlocksList, BlocksSVAs, BlocksSizes, &size, actualSize);
	if(chk != 1) panic("alloc_block_BF: WRONG BF ALLOC .. FreeMemBlocksList content is not correct.");
	if(size != actualSize) panic("alloc_block_BF: WRONG BF ALLOC .. FreeMemBlocksList size is not correct.");

	//====================================================================//
	/*BF ALLOC Scenario 3: Try to allocate a block with a size smaller than the size of any existing free block (ONLY ONE BLOCK CAN FIT)*/
	blockToAlloc = alloc_block_BF(1*Mega);

	//Check returned block content
	if(blockToAlloc == NULL || blockToAlloc->size != 1*Mega || blockToAlloc->sva != 0x1A400)
		panic("alloc_block_BF: WRONG BF ALLOC - alloc_block_BF find a wrong block.");

	//Check size of AvailableMemBlocksList, FreeMemBlocksList & AllocMemBlocksList
	if (LIST_SIZE(&(AvailableMemBlocksList)) != numOfBlocks-1 || LIST_SIZE(&(FreeMemBlocksList)) != numOfFreeBlocks_BF-1 || LIST_SIZE(&(AllocMemBlocksList)) != 0)
		panic("alloc_block_BF: Wrong sizes for AvailableMemBlocksList, FreeMemBlocksList and AllocMemBlocksList.");

	//Check FreeMemBlocksList content
	size = 0;
	actualSize = numOfFreeBlocks_BF-1;
	BlocksSVAs[5] = 0x11A400;  //105KB + 1Mega = (105*1024)+(1024*1024)
	BlocksSizes[5] = 1*Mega;
	chk = check_list_data(&FreeMemBlocksList, BlocksSVAs, BlocksSizes, &size, actualSize);
	if(chk != 1) panic("alloc_block_BF: WRONG BF ALLOC .. FreeMemBlocksList content is not correct.");
	if(size != actualSize) panic("alloc_block_BF: WRONG BF ALLOC .. FreeMemBlocksList size is not correct.");

	//====================================================================//
	/*BF ALLOC Scenario 4: Try to allocate a block with a size smaller than the size of any existing free block (MORE THAN ONE BLOCK CAN FIT)*/
	blockToAlloc = alloc_block_BF(3*kilo);

	//Check returned block content
	if(blockToAlloc == NULL || blockToAlloc->size != 3*kilo || blockToAlloc->sva != 0x19000)
		panic("alloc_block_BF: WRONG BF ALLOC - alloc_block_BF find a wrong block.");

	//Check size of AvailableMemBlocksList, FreeMemBlocksList & AllocMemBlocksList
	if (LIST_SIZE(&(AvailableMemBlocksList)) != numOfBlocks-2 || LIST_SIZE(&(FreeMemBlocksList)) != numOfFreeBlocks_BF-1 || LIST_SIZE(&(AllocMemBlocksList)) != 0)
		panic("alloc_block_BF: Wrong sizes for AvailableMemBlocksList, FreeMemBlocksList and AllocMemBlocksList.");

	//Check FreeMemBlocksList content
	size = 0;
	actualSize = numOfFreeBlocks_BF-1;
	BlocksSVAs[4] = 0x19C00;  //100KB + 3KB
	BlocksSizes[4] = 1*kilo;
	chk = check_list_data(&FreeMemBlocksList, BlocksSVAs, BlocksSizes, &size, actualSize);
	if(chk != 1) panic("alloc_block_BF: WRONG BF ALLOC .. FreeMemBlocksList content is not correct.");
	if(size != actualSize) panic("alloc_block_BF: WRONG BF ALLOC .. FreeMemBlocksList size is not correct.");

	//====================================================================//
	/*BF ALLOC Scenario 5: Try to allocate a block @ the head of the free block list */
	blockToAlloc = alloc_block_BF(2*kilo);

	//Check returned block content
	if(blockToAlloc == NULL || blockToAlloc->size != 2*kilo || blockToAlloc->sva != 0x0)
		panic("alloc_block_BF: WRONG BF ALLOC - alloc_block_BF find a wrong block.");

	//Check size of AvailableMemBlocksList, FreeMemBlocksList & AllocMemBlocksList
	if (LIST_SIZE(&(AvailableMemBlocksList)) != numOfBlocks-2 || LIST_SIZE(&(FreeMemBlocksList)) != numOfFreeBlocks_BF-2 || LIST_SIZE(&(AllocMemBlocksList)) != 0)
		panic("alloc_block_BF: Wrong sizes for AvailableMemBlocksList, FreeMemBlocksList and AllocMemBlocksList.");

	//Check FreeMemBlocksList content
	size = 0;
	actualSize = numOfFreeBlocks_BF-2;
	BlocksSVAs[0] = 0x0;
	BlocksSizes[0] = 0;
	chk = check_list_data(&FreeMemBlocksList, BlocksSVAs, BlocksSizes, &size, actualSize);
	if(chk != 1) panic("alloc_block_BF: WRONG BF ALLOC .. FreeMemBlocksList content is not correct.");
	if(size != actualSize) panic("alloc_block_BF: WRONG BF ALLOC .. FreeMemBlocksList size is not correct.");

	//====================================================================//
	//show_list_content(&FreeMemBlocksList);
	cprintf("Congratulations!! test alloc_block_BF completed successfully.\n");
}

void test_alloc_block_NF()
{
	uint32 numOfBlocks = 10;
	initialize_MemBlocksList(numOfBlocks);
	LIST_INIT(&AllocMemBlocksList);

	//Fill AllocMemBlocksList with multiple blocks
	#define numOfFreeBlocks_NF 8
	uint32 BlocksSVAs[numOfFreeBlocks_NF] = {0x0, 0x2000, 0x4000, 0x7000, 0x19000, 0x1A400, 0x21B400, 0x31B400}; //0KB, 8KB, 16KB, 28KB, 100KB, 105KB, 2,157KB, 3,181KB
	uint32 BlocksSizes[numOfFreeBlocks_NF] = {3*kilo, 8*kilo, 1*kilo, 10*kilo, 4*kilo, 2*Mega, 1*Mega, 1*kilo} ;

	LIST_INIT(&FreeMemBlocksList);
	struct MemBlock freeBlocks[numOfFreeBlocks_NF];
	for (int i = 0; i < numOfFreeBlocks_NF; ++i)
	{
		freeBlocks[i].sva = BlocksSVAs[i];
		freeBlocks[i].size = BlocksSizes[i];
		if (i == 0)
			LIST_INSERT_HEAD(&FreeMemBlocksList, &(freeBlocks[i]));
		else
			LIST_INSERT_TAIL(&FreeMemBlocksList, &(freeBlocks[i]));
	}

	//====================================================================//
	/*NF ALLOC Scenario 1: Try to allocate a block with a size greater than the size of any existing free block*/
	struct MemBlock * blockToAlloc = alloc_block_NF(10*Mega);

	//Check returned block content
	if(blockToAlloc != NULL) panic("alloc_block_NF: WRONG NF ALLOC - alloc_block_NF find a block instead no existing free blocks with the given size.");

	//Check size of AvailableMemBlocksList, FreeMemBlocksList & AllocMemBlocksList
	if (LIST_SIZE(&(AvailableMemBlocksList)) != numOfBlocks || LIST_SIZE(&(FreeMemBlocksList)) != numOfFreeBlocks_NF || LIST_SIZE(&(AllocMemBlocksList)) != 0)
		panic("find_block: AvailableMemBlocksList, FreeMemBlocksList & AllocMemBlocksList are affected and this shall not occurred.");

	//====================================================================//
	/*NF ALLOC Scenario 2: Try to allocate a block with a size equal to the size of the one existing free blocks (STARTING from 0)*/
	blockToAlloc = alloc_block_NF(10*kilo);

	//Check returned block content
	if(blockToAlloc == NULL || blockToAlloc->size != 10*kilo || blockToAlloc->sva != 0x7000)
		panic("alloc_block_NF: WRONG NF ALLOC - alloc_block_NF find a wrong block.");

	//Check size of AvailableMemBlocksList, FreeMemBlocksList & AllocMemBlocksList
	if (LIST_SIZE(&(AvailableMemBlocksList)) != numOfBlocks || LIST_SIZE(&(FreeMemBlocksList)) != numOfFreeBlocks_NF-1 || LIST_SIZE(&(AllocMemBlocksList)) != 0)
		panic("alloc_block_NF: Wrong sizes for AvailableMemBlocksList, FreeMemBlocksList and AllocMemBlocksList.");

	//Check FreeMemBlocksList content
	uint32 size = 0;
	uint32 actualSize = numOfFreeBlocks_NF-1;
	BlocksSVAs[3] = 0;
	BlocksSizes[3] = 0;
	int chk = check_list_data(&FreeMemBlocksList, BlocksSVAs, BlocksSizes, &size, actualSize);
	if(chk != 1) panic("alloc_block_NF: WRONG NF ALLOC .. FreeMemBlocksList content is not correct.");
	if(size != actualSize) panic("alloc_block_NF: WRONG NF ALLOC .. FreeMemBlocksList size is not correct.");

	//====================================================================//
	/*NF ALLOC Scenario 3: Try to allocate a block with a size equal to the size of the one existing free blocks (The first one fit after the last allocated VA)*/
	// Now, lastAllocVA shall EQUAL to 0x7000
	blockToAlloc = alloc_block_NF(2*Mega);

	//Check returned block content
	if(blockToAlloc == NULL || blockToAlloc->size != 2*Mega || blockToAlloc->sva != 0x1A400)
		panic("alloc_block_NF: WRONG NF ALLOC - alloc_block_NF find a wrong block.");

	//Check size of AvailableMemBlocksList, FreeMemBlocksList & AllocMemBlocksList
	if (LIST_SIZE(&(AvailableMemBlocksList)) != numOfBlocks || LIST_SIZE(&(FreeMemBlocksList)) != numOfFreeBlocks_NF-2 || LIST_SIZE(&(AllocMemBlocksList)) != 0)
		panic("alloc_block_NF: Wrong sizes for AvailableMemBlocksList, FreeMemBlocksList and AllocMemBlocksList.");

	//Check FreeMemBlocksList content
	size = 0;
	actualSize = numOfFreeBlocks_NF-2;
	BlocksSVAs[5] = 0;
	BlocksSizes[5] = 0;
	chk = check_list_data(&FreeMemBlocksList, BlocksSVAs, BlocksSizes, &size, actualSize);
	if(chk != 1) panic("alloc_block_NF: WRONG NF ALLOC .. FreeMemBlocksList content is not correct.");
	if(size != actualSize) panic("alloc_block_NF: WRONG NF ALLOC .. FreeMemBlocksList size is not correct.");

	//====================================================================//
	/*NF ALLOC Scenario 4: Try to allocate a block with a size smaller than the size of any existing free block (The first one fit after the last allocated VA)*/
	// Now, lastAllocVA shall EQUAL to 0x7000
	blockToAlloc = alloc_block_NF(512*kilo);

	//Check returned block content
	if(blockToAlloc == NULL || blockToAlloc->size != 512*kilo || blockToAlloc->sva != 0x21B400)
		panic("alloc_block_NF: WRONG NF ALLOC - alloc_block_NF find a wrong block.");

	//Check size of AvailableMemBlocksList, FreeMemBlocksList & AllocMemBlocksList
	if (LIST_SIZE(&(AvailableMemBlocksList)) != numOfBlocks-1 || LIST_SIZE(&(FreeMemBlocksList)) != numOfFreeBlocks_NF-2 || LIST_SIZE(&(AllocMemBlocksList)) != 0)
		panic("alloc_block_NF: Wrong sizes for AvailableMemBlocksList, FreeMemBlocksList and AllocMemBlocksList.");

	//Check FreeMemBlocksList content
	size = 0;
	actualSize = numOfFreeBlocks_NF-2;
	BlocksSVAs[6] = 0x21B400 + 512*kilo;
	BlocksSizes[6] = 512*kilo;
	chk = check_list_data(&FreeMemBlocksList, BlocksSVAs, BlocksSizes, &size, actualSize);
	if(chk != 1) panic("alloc_block_NF: WRONG NF ALLOC .. FreeMemBlocksList content is not correct.");
	if(size != actualSize) panic("alloc_block_NF: WRONG NF ALLOC .. FreeMemBlocksList size is not correct.");

	//====================================================================//
	/*NF ALLOC Scenario 5: Try to allocate a block with a size smaller than the size of any existing free block (One from the updated blocks before in the free list)*/
	// Now, lastAllocVA shall EQUAL to 0x7000
	blockToAlloc = alloc_block_NF(511*kilo);

	//Check returned block content
	if(blockToAlloc == NULL || blockToAlloc->size != 511*kilo || blockToAlloc->sva != (0x21B400 + 512*kilo))
		panic("alloc_block_NF: WRONG NF ALLOC - alloc_block_NF find a wrong block.");

	//Check size of AvailableMemBlocksList, FreeMemBlocksList & AllocMemBlocksList
	if (LIST_SIZE(&(AvailableMemBlocksList)) != numOfBlocks-2 || LIST_SIZE(&(FreeMemBlocksList)) != numOfFreeBlocks_NF-2 || LIST_SIZE(&(AllocMemBlocksList)) != 0)
		panic("alloc_block_NF: Wrong sizes for AvailableMemBlocksList, FreeMemBlocksList and AllocMemBlocksList.");

	//Check FreeMemBlocksList content
	size = 0;
	actualSize = numOfFreeBlocks_NF-2;
	BlocksSVAs[6] = 0x21B400 + 512*kilo + (511*kilo);
	BlocksSizes[6] = 1*kilo;
	chk = check_list_data(&FreeMemBlocksList, BlocksSVAs, BlocksSizes, &size, actualSize);
	if(chk != 1) panic("alloc_block_NF: WRONG NF ALLOC .. FreeMemBlocksList content is not correct.");
	if(size != actualSize) panic("alloc_block_NF: WRONG NF ALLOC .. FreeMemBlocksList size is not correct.");

	//====================================================================//
	/*NF ALLOC Scenario 6: Try to allocate a block with a size smaller than ALL the NEXT existing blocks .. Shall start search from the start of the list*/
	// Now, lastAllocVA shall EQUAL to 0x7000
	blockToAlloc = alloc_block_NF(3*kilo);

	//Check returned block content
	if(blockToAlloc == NULL || blockToAlloc->size != 3*kilo || blockToAlloc->sva != 0x0)
		panic("alloc_block_NF: WRONG NF ALLOC - alloc_block_NF find a wrong block.");

	//Check size of AvailableMemBlocksList, FreeMemBlocksList & AllocMemBlocksList
	if (LIST_SIZE(&(AvailableMemBlocksList)) != numOfBlocks-2 || LIST_SIZE(&(FreeMemBlocksList)) != numOfFreeBlocks_NF-3 || LIST_SIZE(&(AllocMemBlocksList)) != 0)
		panic("alloc_block_NF: Wrong sizes for AvailableMemBlocksList, FreeMemBlocksList and AllocMemBlocksList.");

	//Check FreeMemBlocksList content
	size = 0;
	actualSize = numOfFreeBlocks_NF-3;
	BlocksSVAs[0] = 0;
	BlocksSizes[0] = 0;
	chk = check_list_data(&FreeMemBlocksList, BlocksSVAs, BlocksSizes, &size, actualSize);
	if(chk != 1) panic("alloc_block_NF: WRONG NF ALLOC .. FreeMemBlocksList content is not correct.");
	if(size != actualSize) panic("alloc_block_NF: WRONG NF ALLOC .. FreeMemBlocksList size is not correct.");

	//====================================================================//
	/*NF ALLOC Scenario 7: Try to allocate a block with a size smaller than the existing blocks .. To try to update head not to remove it*/
	// Now, lastAllocVA shall EQUAL to 0x7000
	blockToAlloc = alloc_block_NF(1*kilo);

	//Check returned block content
	if(blockToAlloc == NULL || blockToAlloc->size != 1*kilo || blockToAlloc->sva != 0x2000)
		panic("alloc_block_NF: WRONG NF ALLOC - alloc_block_NF find a wrong block.");

	//Check size of AvailableMemBlocksList, FreeMemBlocksList & AllocMemBlocksList
	if (LIST_SIZE(&(AvailableMemBlocksList)) != numOfBlocks-3 || LIST_SIZE(&(FreeMemBlocksList)) != numOfFreeBlocks_NF-3 || LIST_SIZE(&(AllocMemBlocksList)) != 0)
		panic("alloc_block_NF: Wrong sizes for AvailableMemBlocksList, FreeMemBlocksList and AllocMemBlocksList.");

	//Check FreeMemBlocksList content
	size = 0;
	actualSize = numOfFreeBlocks_NF-3;
	BlocksSVAs[1] = 0x2000 + 1*kilo;
	BlocksSizes[1] = 7*kilo;
	chk = check_list_data(&FreeMemBlocksList, BlocksSVAs, BlocksSizes, &size, actualSize);
	if(chk != 1) panic("alloc_block_NF: WRONG NF ALLOC .. FreeMemBlocksList content is not correct.");
	if(size != actualSize) panic("alloc_block_NF: WRONG NF ALLOC .. FreeMemBlocksList size is not correct.");

	//====================================================================//
	//show_list_content(&FreeMemBlocksList);
	cprintf("Congratulations!! test alloc_block_NF completed successfully.\n");

}

void test_insert_sorted_with_merge_freeList()
{
	uint32 numOfBlocks = 10;
	initialize_MemBlocksList(numOfBlocks);
	LIST_INIT(&AllocMemBlocksList);
	LIST_INIT(&FreeMemBlocksList);

	uint32 blocksToInsertSVAs[10]={0,0,0,0,0,0,0,0,0,0};
	uint32 blocksToInsertSizes[10]={0,0,0,0,0,0,0,0,0,0};
	uint32 size = 0;
	uint32 actualSize = 0;

	/*INSERT_SORTED_WITH MERGE Scenario 1: CASE 0 - Insert BLOCK_1 in the empty FreeMemBlocksList*/
	int idx_blocksToInsert = 3;
	blocksToInsertSVAs[idx_blocksToInsert] = 8*kilo;
	blocksToInsertSizes[idx_blocksToInsert] = 4*kilo;
	struct MemBlock blockToInsert1;
	blockToInsert1.sva = blocksToInsertSVAs[idx_blocksToInsert];
	blockToInsert1.size = blocksToInsertSizes[idx_blocksToInsert];

	insert_sorted_with_merge_freeList(&blockToInsert1);

	//Check 1: Check size of the AllocMemBlocksList
	if (LIST_SIZE(&(AllocMemBlocksList)) != 0 || LIST_SIZE(&(AvailableMemBlocksList)) != numOfBlocks || LIST_SIZE(&(FreeMemBlocksList)) != 1)
		panic("insert_sorted_with_merge: Wrong sizes for AvailableMemBlocksList, FreeMemBlocksList and AllocMemBlocksList.");

	//Check 2: Check the free list content
	size = 0;
	actualSize = 1;
	int chk = check_list_data(&FreeMemBlocksList, blocksToInsertSVAs, blocksToInsertSizes, &size, actualSize);
	if(chk != 1) panic("insert_sorted_with_merge: WRONG INSERT .. FreeMemBlocksList content is not correct.");
	if(size != actualSize) panic("insert_sorted_with_merge: WRONG INSERT .. FreeMemBlocksList size is not correct.");

	//====================================================================//
	/*INSERT_SORTED_WITH MERGE Scenario 2: CASE 1 - Insert with NO MERGE in the FreeMemBlocksList (AT THE TAIL)*/
	idx_blocksToInsert = 7;
	blocksToInsertSVAs[idx_blocksToInsert] = 8*kilo + 4*kilo + 20*kilo;
	blocksToInsertSizes[idx_blocksToInsert] = 7*kilo;
	struct MemBlock blockToInsert2;
	blockToInsert2.sva = blocksToInsertSVAs[idx_blocksToInsert];
	blockToInsert2.size = blocksToInsertSizes[idx_blocksToInsert];

	insert_sorted_with_merge_freeList(&blockToInsert2);

	//Check 1: Check size of the AllocMemBlocksList
	if (LIST_SIZE(&(AllocMemBlocksList)) != 0 || LIST_SIZE(&(AvailableMemBlocksList)) != numOfBlocks || LIST_SIZE(&(FreeMemBlocksList)) != 2)
		panic("insert_sorted_with_merge: Wrong sizes for AvailableMemBlocksList, FreeMemBlocksList and AllocMemBlocksList.");

	//Check 2: Check the FreeMemBlocksList content
	size = 0;
	actualSize = 2;
	chk = check_list_data(&FreeMemBlocksList, blocksToInsertSVAs, blocksToInsertSizes, &size, actualSize);
	if(chk != 1) panic("insert_sorted_with_merge: WRONG INSERT .. FreeMemBlocksList content is not correct.");
	if(size != actualSize) panic("insert_sorted_with_merge: WRONG INSERT .. FreeMemBlocksList size is not correct.");

	//====================================================================//
	/*INSERT_SORTED_WITH MERGE Scenario 3: CASE 1 - Insert with NO MERGE in the FreeMemBlocksList (AT THE HEAD)*/
	idx_blocksToInsert = 1;
	blocksToInsertSVAs[idx_blocksToInsert] = 1*kilo;
	blocksToInsertSizes[idx_blocksToInsert] = 1*kilo;
	struct MemBlock blockToInsert3;
	blockToInsert3.sva = blocksToInsertSVAs[idx_blocksToInsert];
	blockToInsert3.size = blocksToInsertSizes[idx_blocksToInsert];

	insert_sorted_with_merge_freeList(&blockToInsert3);

	//Check 1: Check size of the AllocMemBlocksList
	if (LIST_SIZE(&(AllocMemBlocksList)) != 0 || LIST_SIZE(&(AvailableMemBlocksList)) != numOfBlocks || LIST_SIZE(&(FreeMemBlocksList)) != 3)
		panic("insert_sorted_with_merge: Wrong sizes for AvailableMemBlocksList, FreeMemBlocksList and AllocMemBlocksList.");

	//Check 2: Check the FreeMemBlocksList content
	size = 0;
	actualSize = 3;
	chk = check_list_data(&FreeMemBlocksList, blocksToInsertSVAs, blocksToInsertSizes, &size, actualSize);
	if(chk != 1) panic("insert_sorted_with_merge: WRONG INSERT .. FreeMemBlocksList content is not correct.");
	if(size != actualSize) panic("insert_sorted_with_merge: WRONG INSERT .. FreeMemBlocksList size is not correct.");

	//====================================================================//
	/*INSERT_SORTED_WITH MERGE Scenario 4: CASE 1 - Insert with NO MERGE in the FreeMemBlocksList (between 2 blocks)*/
	idx_blocksToInsert = 6;
	blocksToInsertSVAs[idx_blocksToInsert] = 8*kilo + 4*kilo + 10*kilo;
	blocksToInsertSizes[idx_blocksToInsert] = 4*kilo;
	struct MemBlock blockToInsert4;
	blockToInsert4.sva = blocksToInsertSVAs[idx_blocksToInsert];
	blockToInsert4.size = blocksToInsertSizes[idx_blocksToInsert];

	insert_sorted_with_merge_freeList(&blockToInsert4);

	//Check 1: Check size of the AllocMemBlocksList
	if (LIST_SIZE(&(AllocMemBlocksList)) != 0 || LIST_SIZE(&(AvailableMemBlocksList)) != numOfBlocks || LIST_SIZE(&(FreeMemBlocksList)) != 4)
		panic("insert_sorted_with_merge: Wrong sizes for AvailableMemBlocksList, FreeMemBlocksList and AllocMemBlocksList.");

	//Check 2: Check the FreeMemBlocksList content
	size = 0;
	actualSize = 4;
	chk = check_list_data(&FreeMemBlocksList, blocksToInsertSVAs, blocksToInsertSizes, &size, actualSize);
	if(chk != 1) panic("insert_sorted_with_merge: WRONG INSERT .. FreeMemBlocksList content is not correct.");
	if(size != actualSize) panic("insert_sorted_with_merge: WRONG INSERT .. FreeMemBlocksList size is not correct.");

	//====================================================================//
	/*INSERT_SORTED_WITH MERGE Scenario 5: CASE 2 - Merge with previous ONLY in the FreeMemBlocksList (AT the tail)*/
	idx_blocksToInsert = 8;
	blocksToInsertSVAs[idx_blocksToInsert] = 8*kilo + 4*kilo + 20*kilo + 7*kilo;
	blocksToInsertSizes[idx_blocksToInsert] = 3*kilo;
	struct MemBlock blockToInsert5;
	blockToInsert5.sva = blocksToInsertSVAs[idx_blocksToInsert];
	blockToInsert5.size = blocksToInsertSizes[idx_blocksToInsert];

//	cprintf("===>BEFORE\n");
//	show_list_content(&FreeMemBlocksList);

	insert_sorted_with_merge_freeList(&blockToInsert5);

	//Check 1: Check size of the AllocMemBlocksList
	if (LIST_SIZE(&(AllocMemBlocksList)) != 0 || LIST_SIZE(&(AvailableMemBlocksList)) != numOfBlocks + 1 || LIST_SIZE(&(FreeMemBlocksList)) != 4)
		panic("insert_sorted_with_merge: Wrong sizes for AvailableMemBlocksList, FreeMemBlocksList and AllocMemBlocksList.");

	//Check 2: Check the FreeMemBlocksList content
	size = 0;
	actualSize = 4;
	blocksToInsertSizes[idx_blocksToInsert-1] += blocksToInsertSizes[idx_blocksToInsert];
	blocksToInsertSVAs[idx_blocksToInsert] = 0x0;
	blocksToInsertSizes[idx_blocksToInsert] = 0;
	chk = check_list_data(&FreeMemBlocksList, blocksToInsertSVAs, blocksToInsertSizes, &size, actualSize);
	if(chk != 1) panic("insert_sorted_with_merge: WRONG INSERT .. FreeMemBlocksList content is not correct.");
	if(size != actualSize) panic("insert_sorted_with_merge: WRONG INSERT .. FreeMemBlocksList size is not correct.");

	//Check 3: Check the AvailableMemBlocksList content
	size = 0;
	chk = check_avaliable_list_data(&size);
	if(size != numOfBlocks + 1) panic("insert_sorted_with_merge: WRONG INSERT .. AvailableMemBlocksList size is not correct.");
	if(chk != 1) panic("insert_sorted_with_merge: WRONG INSERT .. AvailableMemBlocksList content is not correct.");

	//====================================================================//
	/*INSERT_SORTED_WITH MERGE Scenario 6: CASE 2 - Merge with previous ONLY in the FreeMemBlocksList (between 2 blocks)*/
	idx_blocksToInsert = 4;
	blocksToInsertSVAs[idx_blocksToInsert] = 8*kilo + 4*kilo;
	blocksToInsertSizes[idx_blocksToInsert] = 1*kilo;
	struct MemBlock blockToInsert6;
	blockToInsert6.sva = blocksToInsertSVAs[idx_blocksToInsert];
	blockToInsert6.size = blocksToInsertSizes[idx_blocksToInsert];

//	cprintf("===>BEFORE\n");
//	show_list_content(&FreeMemBlocksList);

	insert_sorted_with_merge_freeList(&blockToInsert6);

	//Check 1: Check size of the AllocMemBlocksList
	if (LIST_SIZE(&(AllocMemBlocksList)) != 0 || LIST_SIZE(&(AvailableMemBlocksList)) != numOfBlocks + 2 || LIST_SIZE(&(FreeMemBlocksList)) != 4)
		panic("insert_sorted_with_merge: Wrong sizes for AvailableMemBlocksList, FreeMemBlocksList and AllocMemBlocksList.");

	//Check 2: Check the FreeMemBlocksList content
	size = 0;
	actualSize = 4;
	blocksToInsertSizes[idx_blocksToInsert-1] += blocksToInsertSizes[idx_blocksToInsert];
	blocksToInsertSVAs[idx_blocksToInsert] = 0x0;
	blocksToInsertSizes[idx_blocksToInsert] = 0;
	chk = check_list_data(&FreeMemBlocksList, blocksToInsertSVAs, blocksToInsertSizes, &size, actualSize);
	if(chk != 1) panic("insert_sorted_with_merge: WRONG INSERT .. FreeMemBlocksList content is not correct.");
	if(size != actualSize) panic("insert_sorted_with_merge: WRONG INSERT .. FreeMemBlocksList size is not correct.");

	//Check 3: Check the AvailableMemBlocksList content
	size = 0;
	chk = check_avaliable_list_data(&size);
	if(size != numOfBlocks + 2) panic("insert_sorted_with_merge: WRONG INSERT .. AvailableMemBlocksList size is not correct.");
	if(chk != 1) panic("insert_sorted_with_merge: WRONG INSERT .. AvailableMemBlocksList content is not correct.");

	//====================================================================//
	/*INSERT_SORTED_WITH MERGE Scenario 7: CASE 3 - Merge with next ONLY in the FreeMemBlocksList (AT the head)*/
	idx_blocksToInsert = 0;
	blocksToInsertSVAs[idx_blocksToInsert] = 0x0;
	blocksToInsertSizes[idx_blocksToInsert] = 1*kilo;
	struct MemBlock blockToInsert7;
	blockToInsert7.sva = blocksToInsertSVAs[idx_blocksToInsert];
	blockToInsert7.size = blocksToInsertSizes[idx_blocksToInsert];

//	cprintf("===>BEFORE\n");
//	show_list_content(&FreeMemBlocksList);

	insert_sorted_with_merge_freeList(&blockToInsert7);
//	show_list_content(&FreeMemBlocksList);

	//Check 1: Check size of the AllocMemBlocksList
	if (LIST_SIZE(&(AllocMemBlocksList)) != 0 || LIST_SIZE(&(AvailableMemBlocksList)) != numOfBlocks + 3 || LIST_SIZE(&(FreeMemBlocksList)) != 4)
		panic("insert_sorted_with_merge: Wrong sizes for AvailableMemBlocksList, FreeMemBlocksList and AllocMemBlocksList.");

	//Check 2: Check the FreeMemBlocksList content
	size = 0;
	actualSize = 4;
	blocksToInsertSVAs[idx_blocksToInsert+1] = blocksToInsertSVAs[idx_blocksToInsert];
	blocksToInsertSizes[idx_blocksToInsert+1] += blocksToInsertSizes[idx_blocksToInsert];
	blocksToInsertSVAs[idx_blocksToInsert] = 0x0;
	blocksToInsertSizes[idx_blocksToInsert] = 0;
	chk = check_list_data(&FreeMemBlocksList, blocksToInsertSVAs, blocksToInsertSizes, &size, actualSize);
	if(chk != 1) panic("insert_sorted_with_merge: WRONG INSERT .. FreeMemBlocksList content is not correct.");
	if(size != actualSize) panic("insert_sorted_with_merge: WRONG INSERT .. FreeMemBlocksList size is not correct.");

	//Check 3: Check the AvailableMemBlocksList content
	size = 0;
	chk = check_avaliable_list_data(&size);
	if(size != numOfBlocks + 3) panic("insert_sorted_with_merge: WRONG INSERT .. AvailableMemBlocksList size is not correct.");
	if(chk != 1) panic("insert_sorted_with_merge: WRONG INSERT .. AvailableMemBlocksList content is not correct.");

	//====================================================================//
	/*INSERT_SORTED_WITH MERGE Scenario 8: CASE 3 - Merge with next ONLY in the FreeMemBlocksList (between 2 blocks)*/
	idx_blocksToInsert = 5;
	blocksToInsertSVAs[idx_blocksToInsert] = (8*kilo + 4*kilo + 10*kilo) - (4*kilo);
	blocksToInsertSizes[idx_blocksToInsert] = 4*kilo;
	struct MemBlock blockToInsert8;
	blockToInsert8.sva = blocksToInsertSVAs[idx_blocksToInsert];
	blockToInsert8.size = blocksToInsertSizes[idx_blocksToInsert];

//	cprintf("===>BEFORE\n");
//	show_list_content(&FreeMemBlocksList);

	insert_sorted_with_merge_freeList(&blockToInsert8);
//	show_list_content(&FreeMemBlocksList);

	//Check 1: Check size of the AllocMemBlocksList
	if (LIST_SIZE(&(AllocMemBlocksList)) != 0 || LIST_SIZE(&(AvailableMemBlocksList)) != numOfBlocks + 4 || LIST_SIZE(&(FreeMemBlocksList)) != 4)
		panic("insert_sorted_with_merge: Wrong sizes for AvailableMemBlocksList, FreeMemBlocksList and AllocMemBlocksList.");

	//Check 2: Check the FreeMemBlocksList content
	size = 0;
	actualSize = 4;
	blocksToInsertSVAs[idx_blocksToInsert+1] = blocksToInsertSVAs[idx_blocksToInsert];
	blocksToInsertSizes[idx_blocksToInsert+1] += blocksToInsertSizes[idx_blocksToInsert];
	blocksToInsertSVAs[idx_blocksToInsert] = 0x0;
	blocksToInsertSizes[idx_blocksToInsert] = 0;
	chk = check_list_data(&FreeMemBlocksList, blocksToInsertSVAs, blocksToInsertSizes, &size, actualSize);
	if(chk != 1) panic("insert_sorted_with_merge: WRONG INSERT .. FreeMemBlocksList content is not correct.");
	if(size != actualSize) panic("insert_sorted_with_merge: WRONG INSERT .. FreeMemBlocksList size is not correct.");

	//Check 3: Check the AvailableMemBlocksList content
	size = 0;
	chk = check_avaliable_list_data(&size);
	if(size != numOfBlocks + 4) panic("insert_sorted_with_merge: WRONG INSERT .. AvailableMemBlocksList size is not correct.");
	if(chk != 1) panic("insert_sorted_with_merge: WRONG INSERT .. AvailableMemBlocksList content is not correct.");

	//====================================================================//
	/*INSERT_SORTED_WITH MERGE Scenario 9: CASE 4 - Merge with prev & next in the FreeMemBlocksList */
	idx_blocksToInsert = 2;
	blocksToInsertSVAs[idx_blocksToInsert] = 2*kilo;
	blocksToInsertSizes[idx_blocksToInsert] = 0x2000 - 2*kilo; //6144
	struct MemBlock blockToInsert9;
	blockToInsert9.sva = blocksToInsertSVAs[idx_blocksToInsert];
	blockToInsert9.size = blocksToInsertSizes[idx_blocksToInsert];

	//printf("===>BEFORE\n");
	//show_list_content(&FreeMemBlocksList);

	insert_sorted_with_merge_freeList(&blockToInsert9);
	//show_list_content(&FreeMemBlocksList);

	//Check 1: Check size of the AllocMemBlocksList
	if (LIST_SIZE(&(AllocMemBlocksList)) != 0 || LIST_SIZE(&(AvailableMemBlocksList)) != numOfBlocks + 6 || LIST_SIZE(&(FreeMemBlocksList)) != 3)
		panic("insert_sorted_with_merge: Wrong sizes for AvailableMemBlocksList, FreeMemBlocksList and AllocMemBlocksList.");

	//Check 2: Check the FreeMemBlocksList content
	size = 0;
	actualSize = 3;
	blocksToInsertSVAs[idx_blocksToInsert-1] = 0x0;
	blocksToInsertSizes[idx_blocksToInsert-1] = blocksToInsertSizes[idx_blocksToInsert-1] + blocksToInsertSizes[idx_blocksToInsert] + blocksToInsertSizes[idx_blocksToInsert+1];
	blocksToInsertSVAs[idx_blocksToInsert] = 0x0;
	blocksToInsertSizes[idx_blocksToInsert] = 0;
	blocksToInsertSVAs[idx_blocksToInsert+1] = 0x0;
	blocksToInsertSizes[idx_blocksToInsert+1] = 0;
	chk = check_list_data(&FreeMemBlocksList, blocksToInsertSVAs, blocksToInsertSizes, &size, actualSize);
	if(chk != 1) panic("insert_sorted_with_merge: WRONG INSERT .. FreeMemBlocksList content is not correct.");
	if(size != actualSize) panic("insert_sorted_with_merge: WRONG INSERT .. FreeMemBlocksList size is not correct.");

	//Check 3: Check the AvailableMemBlocksList content
	size = 0;
	chk = check_avaliable_list_data(&size);
	if(size != numOfBlocks + 6) panic("insert_sorted_with_merge: WRONG INSERT .. AvailableMemBlocksList size is not correct.");
	if(chk != 1) panic("insert_sorted_with_merge: WRONG INSERT .. AvailableMemBlocksList content is not correct.");
	//====================================================================//


	//show_list_content(&FreeMemBlocksList);
	cprintf("Congratulations!! test insert_sorted_with_merge_freeList completed successfully.\n");

}

/********************Helper Functions***************************/
int check_list_data(struct MemBlock_List* list, uint32* blocksSVAs, uint32* blocksSizes, uint32* size, uint32 actualSize)
{
	struct MemBlock* node = NULL;
	*size = 0;
	int i = 0;
	LIST_FOREACH(node, list)
	{
		while(blocksSizes[i] == 0) //Empty entry in the given ground truth array .. skip it
			i++;
		if(node->sva != blocksSVAs[(i)] || node->size != blocksSizes[i])
		{
//			cprintf("i = %d .. \n", i);
//			cprintf("Node data: sva = %x, size = %d\n", node->sva, node->size);
//			cprintf("blocks[i] data: sva = %x, size = %d\n", blocksSVAs[(i)], blocksSizes[i]);
			return 0;
		}
		(*size)++;
		i++;
	}
	return 1;
}

int check_avaliable_list_data(uint32* size)
{
	struct MemBlock* node = NULL;
	*size = 0;

	LIST_FOREACH(node, &AvailableMemBlocksList)
	{
		if(node->sva != 0x0 || node->size != 0)
		{
			return 0;
		}
		(*size)++;
	}
	return 1;
}


void show_list_content(struct MemBlock_List* list)
{
	struct MemBlock* node = NULL;
	LIST_FOREACH(node, list)
	{
		cprintf("Node data: sva = %x, size = %d\n", node->sva, node->size);
	}
}



